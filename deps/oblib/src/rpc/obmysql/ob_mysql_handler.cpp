/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#define USING_LOG_PREFIX RPC_OBMYSQL

#include "rpc/obmysql/ob_mysql_handler.h"
#include "lib/compress/zlib/ob_zlib_compressor.h"
#include "rpc/ob_request.h"
#include "rpc/obmysql/ob_mysql_packet.h"
#include "rpc/obmysql/ob_mysql_util.h"
#include "rpc/obmysql/ob_mysql_request_utils.h"
#include "rpc/obmysql/ob_virtual_cs_protocol_processor.h"
#include "rpc/obmysql/ob_2_0_protocol_struct.h"
#include "rpc/obmysql/packet/ompk_handshake.h"
#include "rpc/obmysql/packet/ompk_ok.h"
#include "rpc/obmysql/packet/ompk_handshake_response.h"

using namespace oceanbase::common;
using namespace oceanbase::rpc;
using namespace oceanbase::obmysql;

ObMySQLHandler::ObMySQLHandler(rpc::frame::ObReqDeliver& deliver)
    : mysql_processor_(*this), compress_processor_(*this), ob_2_0_processor_(*this), deliver_(deliver)
{
  EZ_ADD_CB(decode);
  EZ_ADD_CB(encode);
  EZ_ADD_CB(process);
  EZ_ADD_CB(on_connect);
  EZ_ADD_CB(on_disconnect);
  EZ_ADD_CB(cleanup);
}

ObMySQLHandler::~ObMySQLHandler()
{}

void* ObMySQLHandler::decode(easy_message_t* m)
{
  int ret = OB_SUCCESS;
  rpc::ObPacket* pkt = NULL;
  if (OB_ISNULL(m)) {
    ret = OB_ERR_UNEXPECTED;
    LOG_ERROR("easy callback message null pointer", K(m), K(ret));
  } else if (OB_ISNULL(m->input) || OB_ISNULL(m->c) || OB_ISNULL(m->pool)) {
    ret = OB_ERR_UNEXPECTED;
    LOG_ERROR("easy callback message null pointer", K(m->input), K(m->c), K(m->pool), K(ret));
  } else if (OB_ISNULL(m->input->pos) || OB_ISNULL(m->input->last)) {
    ret = OB_ERR_UNEXPECTED;
    LOG_ERROR("easy callback message null pointer", K(m->input->pos), K(m->input->last), K(ret));
  } else {
    ObVirtualCSProtocolProcessor* processor = get_protocol_processor(m->c);
    if (OB_ISNULL(processor)) {
      ret = OB_ERR_UNEXPECTED;
      LOG_ERROR("invalid protocol processor", KP(processor), K(ret));
    } else if (OB_FAIL(processor->decode(m, pkt))) {
      LOG_ERROR("fail to decode", K(ret));
    }
  }

  if (OB_FAIL(ret)) {
    m->status = EASY_ERROR;
  }
  return pkt;
}

int ObMySQLHandler::encode(easy_request_t* r, void* packet)
{
  int ret = EASY_OK;
  if (OB_ISNULL(r) || OB_ISNULL(packet)) {
    LOG_ERROR("invalid argument", K(r), K(packet));
    ret = EASY_ERROR;
  } else {
    easy_buf_t* buf = reinterpret_cast<easy_buf_t*>(packet);
    easy_request_addbuf(r, buf);
  }
  return ret;
}

void do_wakeup(easy_request_t* r)
{
  if (OB_ISNULL(r)) {
    LOG_ERROR("null input request", K(r));
  } else {
    easy_client_wait_t* wait_obj = r->client_wait;
    if (NULL != r->client_wait) {
      r->client_wait = NULL;
      easy_client_wait_wakeup(wait_obj);
    }
  }
}

void do_wakeup_request(easy_request_t* r)
{
  if (OB_ISNULL(r) || OB_ISNULL(r->ms) || OB_ISNULL(r->ms->c) || OB_ISNULL(r->ms->pool) || OB_ISNULL(r->ms->c->pool)) {
    LOG_ERROR("null input", K(r));
  } else {
    easy_atomic_inc(&r->ms->c->pool->ref);
    easy_atomic_inc(&r->ms->pool->ref);
  }
  do_wakeup(r);
}

int ObMySQLHandler::process(easy_request_t* r)
{
  int eret = EASY_OK;
  bool is_going_on = true;
  uint32_t sessid = 0;
  if (OB_NOT_NULL(r) && OB_NOT_NULL(r->ms)) {
    sessid = get_sessid(r->ms->c);
  }
  // check if arguments is valid
  if (OB_ISNULL(r) || OB_ISNULL(r->ipacket)) {
    LOG_ERROR("request is empty", K(r), K(r->ipacket), K(sessid));
    eret = EASY_BREAK;
  } else {
    if (r->retcode != EASY_AGAIN) {
      // Create a request object
      if (NULL != r->ms && NULL != r->ms->pool && NULL != r->ms->c) {
        void* buf = NULL;
        // one mysql compress protocol or ob2.0 protocol's packet may be just a part of
        // mysql protocol packet. so, if needed, we should read more data to decode a
        // complete mysql packet.
        bool need_decode_more = false;
        bool can_reroute_req = false;
        ObVirtualCSProtocolProcessor* processor = get_protocol_processor(r->ms->c);
        if (OB_ISNULL(processor)) {
          eret = EASY_BREAK;
          LOG_ERROR("invalid protocol processor", KP(processor));
        } else if (OB_UNLIKELY(OB_SUCCESS != processor->process(r, need_decode_more))) {
          LOG_ERROR("fail to process", K(sessid));
          eret = EASY_BREAK;
        } else if (need_decode_more) {
          eret = EASY_OK;
        } else if (is_in_ssl_connect_phase(r->ms->c)) {
          // set flag in easy io thread
          r->ms->c->ssl_sm_ = SSM_USE_SSL;
          r->opacket = NULL;
          r->retcode = EASY_OK;
          eret = EASY_OK;
          LOG_INFO("MySQL SSL Request", "sessid", sessid, K(easy_connection_str(r->ms->c)));
        } else if (OB_ISNULL(buf = easy_alloc(r->ms->pool, sizeof(ObRequest)))) {
          RPC_LOG(WARN, "alloc easy memory fail", K(sessid));
          eret = EASY_BREAK;
        } else {
          ObRequest* req = NULL;
          req = new (buf) ObRequest(ObRequest::OB_MYSQL);
          req->set_packet(reinterpret_cast<ObPacket*>(r->ipacket));
          req->set_request(r);
          req->set_receive_timestamp(common::ObTimeUtility::current_time());
          req->set_connection_phase(get_connection_phase(r->ms->c));

          // Then deliver the request we composite above.
          int ret = OB_SUCCESS;
          easy_request_sleeping(r);  // set alloc lock && inc ref count
          if (OB_FAIL(deliver_.deliver(*req))) {
            LOG_WARN("deliver request fail", K(*req), K(sessid), K(ret));
            if (NULL != r->ms->c->pool) {
              easy_atomic_dec(&r->ms->c->pool->ref);
            }
            easy_atomic_dec(&r->ms->pool->ref);
            eret = EASY_ABORT;  // disconnect if deliver mysql request fail.
          } else {
            eret = EASY_AGAIN;
          }
        }
      } else {
        eret = EASY_BREAK;
        LOG_ERROR("invalid easy message", K(r->ms), K(sessid));
      }

    } else {
      // wakeup request thread called when send result set sync.
      if (NULL != r->client_wait) {
        if (NULL != r->ms && NULL != r->ms->c) {
          if (r->ms->c->conn_has_error && NULL != r->ms->c->pool) {
            r->client_wait->status = EASY_CONN_CLOSE;
            do_wakeup(r);
            LOG_INFO("process wakeup c has error", "ref", (int)(r->ms->c->pool->ref), K(sessid));
          } else {
            do_wakeup_request(r);
            if (NULL != r->ms->c->pool) {
              LOG_DEBUG("process wakeup", "ref", (int)(r->ms->c->pool->ref), K(sessid));
            }
          }
        } else {
          eret = EASY_BREAK;
          LOG_ERROR("invalid easy message", K(r->ms));
        }
      } else {
        LOG_WARN("client_wait is NULL, connection may be destroyed", K(sessid));
      }
      eret = EASY_AGAIN;
    }
  }

  return eret;
}

int ObMySQLHandler::on_connect(easy_connection_t* c)
{
  int ret = EASY_OK;
  if (OB_ISNULL(c)) {
    ret = EASY_ERROR;
    LOG_ERROR("invalid easy connection", K(c));
  }
  return ret;
}

int ObMySQLHandler::on_disconnect(easy_connection_t* c)
{
  int ret = EASY_OK;
  if (OB_ISNULL(c)) {
    LOG_ERROR("invalid argument", K(c));
    ret = EASY_ERROR;
  } else {
    LOG_INFO("connection disconnect", K(easy_connection_str(c)));
    if (c->reconn_fail > 10) {
      c->auto_reconn = 0;
    }
  }
  return ret;
}

char* ObMySQLHandler::easy_alloc(easy_pool_t* pool, int64_t size) const
{
  char* buf = NULL;
  if (OB_ISNULL(pool)) {
    LOG_WARN("invalid easy pool", K(pool));
  } else {
    if (size > UINT32_MAX || size < 0) {
      LOG_WARN("easy alloc fail", K(size));
    } else {
      buf = static_cast<char*>(easy_pool_alloc(pool, static_cast<uint32_t>(size)));
    }
  }
  return buf;
}

int ObMySQLHandler::send_handshake(int fd, const OMPKHandshake& hsp) const
{
  int ret = OB_SUCCESS;
  static const int64_t MAX_HSPKT_SIZE = 128;
  char buf[MAX_HSPKT_SIZE];
  const int64_t len = MAX_HSPKT_SIZE;
  int64_t pos = 0;
  int64_t pkt_count = 0;

  if (OB_FAIL(hsp.encode(buf, len, pos, pkt_count))) {
    LOG_WARN("encode handshake packet fail", K(ret));
  } else if (OB_UNLIKELY(pkt_count <= 0)) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("invalid pkt count", K(pkt_count), K(ret));
  } else if (OB_FAIL(write_data(fd, buf, pos))) {
    LOG_WARN("write handshake packet data fail", K(ret));
  }

  return ret;
}

int ObMySQLHandler::write_data(int fd, char* buffer, size_t length) const
{
  int ret = OB_SUCCESS;
  if (fd < 0 || OB_ISNULL(buffer) || length <= 0) {
    LOG_ERROR("invalid argument", K(fd), K(buffer), K(length));
    ret = OB_ERROR;
  } else {
    const char* buff = buffer;
    ssize_t count = 0;
    bool is_going_on = true;
    while (OB_SUCC(ret) && length > 0 && (count = write(fd, buff, length)) != 0) {
      is_going_on = true;
      if (-1 == count) {
        if (errno == EINTR) {
          is_going_on = false;  // continue;
        } else {
          ret = OB_ERROR;
          LOG_WARN("write data failed", K(errno), KERRMSG);
        }
      }
      if (is_going_on) {
        buff += count;
        length -= count;
      }
    }
  }

  return ret;
}

int ObMySQLHandler::read_data(int fd, char* buffer, size_t length) const
{
  int ret = OB_SUCCESS;
  static const int64_t timeout = 1000000;  // 1s
  if (fd < 0 || OB_ISNULL(buffer) || length <= 0) {
    LOG_ERROR("invalid argument", K(fd), K(buffer), K(length));
    ret = OB_ERROR;
  } else {
    char* buff = buffer;
    ssize_t count = 0;
    int64_t trycount = 0;
    int64_t start_time = ::oceanbase::common::ObTimeUtility::current_time();
    bool is_going_on = true;
    while (OB_SUCC(ret) && length > 0 && (count = read(fd, buff, length)) != 0) {
      is_going_on = true;
      trycount++;
      if (trycount % 100 == 0 && ::oceanbase::common::ObTimeUtility::current_time() - start_time > timeout) {
        LOG_WARN("timeout", K(fd), K(trycount), K(count), K(length), K(errno), KERRMSG, K(start_time));
        ret = OB_ERROR;
      }
      if (OB_SUCC(ret)) {
        if (-1 == count) {
          if (errno == EINTR || errno == EAGAIN) {
            is_going_on = false;  // continue;
          } else {
            ret = OB_ERROR;
            LOG_WARN("read data failed", K(errno), KERRMSG);
          }
        }
        if (is_going_on) {
          buff += count;
          length -= count;
        }
      }
    }
    if (0 != length) {
      ret = OB_ERROR;
      LOG_WARN("read not return enough data need more bytes", K(length));
    }
  }
  return ret;
}

int ObMySQLHandler::cleanup(easy_request_t* r, void* apacket)
{
  int ret = EASY_OK;
  UNUSED(apacket);
  if (OB_ISNULL(r) || OB_ISNULL(r->client_wait)) {
    // nothing to wakeup, do not return error to caller
  } else {
    r->client_wait->status = EASY_CONN_CLOSE;
    do_wakeup(r);
  }
  return ret;
}

inline ObVirtualCSProtocolProcessor* ObMySQLHandler::get_protocol_processor(easy_connection_t* c)
{
  ObVirtualCSProtocolProcessor* processor = NULL;
  ObCSProtocolType ptype = get_cs_protocol_type(c);
  switch (ptype) {
    case OB_MYSQL_CS_TYPE: {
      processor = &mysql_processor_;
      break;
    }
    case OB_MYSQL_COMPRESS_CS_TYPE: {
      processor = &compress_processor_;
      break;
    }
    case OB_2_0_CS_TYPE: {
      processor = &ob_2_0_processor_;
      break;
    }
    default: {
      LOG_ERROR("invalid cs protocol type", K(ptype));
      break;
    }
  }
  return processor;
}
