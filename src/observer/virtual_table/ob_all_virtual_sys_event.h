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

#ifndef OB_ALL_VIRTUAL_SYS_EVENT_H_
#define OB_ALL_VIRTUAL_SYS_EVENT_H_

#include "share/ob_virtual_table_scanner_iterator.h"
#include "lib/statistic_event/ob_stat_class.h"
#include "lib/statistic_event/ob_stat_event.h"
#include "lib/stat/ob_session_stat.h"
#include "observer/virtual_table/ob_all_virtual_diag_index_scan.h"

namespace oceanbase {
namespace observer {

class ObAllVirtualSysEvent : public common::ObVirtualTableScannerIterator {
public:
  ObAllVirtualSysEvent();
  virtual ~ObAllVirtualSysEvent();
  virtual int inner_get_next_row(common::ObNewRow*& row);
  virtual void reset();
  inline void set_addr(common::ObAddr& addr)
  {
    addr_ = &addr;
  }
  virtual int set_ip(common::ObAddr* addr);

protected:
  virtual int get_all_diag_info();
  common::ObSEArray<std::pair<uint64_t, common::ObDiagnoseTenantInfo*>, common::OB_MAX_SERVER_TENANT_CNT> tenant_dis_;

private:
  enum SYS_COLUMN {
    TENANT_ID = common::OB_APP_MIN_COLUMN_ID,
    SVR_IP,
    SVR_PORT,
    EVENT_ID,
    EVENT,
    WAIT_CLASS_ID,
    WAIT_CLASS_NO,
    WAIT_CLASS,
    TOTAL_WAITS,
    TOTAL_TIMEOUTS,
    TIME_WAITED,
    MAX_WAIT,
    AVERAGE_WAIT,
    TIME_WAITED_MICRO
  };
  common::ObAddr* addr_;
  common::ObString ipstr_;
  int32_t port_;
  int32_t iter_;
  int32_t event_iter_;
  uint64_t tenant_id_;
  DISALLOW_COPY_AND_ASSIGN(ObAllVirtualSysEvent);
};

class ObAllVirtualSysEventI1 : public ObAllVirtualSysEvent, public ObAllVirtualDiagIndexScan {
public:
  ObAllVirtualSysEventI1()
  {}
  virtual ~ObAllVirtualSysEventI1()
  {}
  virtual int inner_open() override
  {
    return set_index_ids(key_ranges_);
  }

protected:
  virtual int get_all_diag_info();

private:
  DISALLOW_COPY_AND_ASSIGN(ObAllVirtualSysEventI1);
};

} /* namespace observer */
} /* namespace oceanbase */

#endif /* OB_ALL_VIRTUAL_SYS_EVENT_H_ */
