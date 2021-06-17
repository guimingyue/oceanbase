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

#include <gtest/gtest.h>
#define private public
#include "lib/stat/ob_diagnose_info.h"
#include "lib/stat/ob_session_stat.h"
#include "lib/lock/ob_spin_rwlock.h"
#include "lib/coro/testing.h"

namespace oceanbase {
namespace common {
#define TENANT_EVENT_GET(stat_no)                                                               \
  ({                                                                                            \
    int64_t ret = 0;                                                                            \
    oceanbase::common::ObDiagnoseTenantInfo* tenant_info =                                      \
        oceanbase::common::ObDiagnoseTenantInfo::get_local_diagnose_info();                     \
    if (NULL != tenant_info) {                                                                  \
      if (oceanbase::common::stat_no < oceanbase::common::ObStatEventIds::STAT_EVENT_ADD_END) { \
        oceanbase::common::ObStatEventAddStat* stat =                                           \
            tenant_info->get_add_stat_stats().get(::oceanbase::common::stat_no);                \
        if (NULL != stat) {                                                                     \
          ret = stat->get_stat_value();                                                         \
        }                                                                                       \
      }                                                                                         \
    }                                                                                           \
    ret;                                                                                        \
  })

TEST(ObDiagnoseSessionInfo, guard)
{
  EVENT_INC(ELECTION_CHANGE_LEAER_COUNT);
  EXPECT_EQ(1, ObDITls<ObSessionDIBuffer>::get_instance()->get_tenant_id());
  EXPECT_EQ(1, TENANT_EVENT_GET(ObStatEventIds::ELECTION_CHANGE_LEAER_COUNT));
  {
    ObTenantStatEstGuard tenant_guard(2);
    EVENT_INC(ELECTION_CHANGE_LEAER_COUNT);
    EXPECT_EQ(2, ObDITls<ObSessionDIBuffer>::get_instance()->get_tenant_id());
    EXPECT_EQ(1, TENANT_EVENT_GET(ObStatEventIds::ELECTION_CHANGE_LEAER_COUNT));
    {
      ObTenantStatEstGuard tenant_guard(3);
      EVENT_INC(ELECTION_CHANGE_LEAER_COUNT);
      EXPECT_EQ(3, ObDITls<ObSessionDIBuffer>::get_instance()->get_tenant_id());
      EXPECT_EQ(1, TENANT_EVENT_GET(ObStatEventIds::ELECTION_CHANGE_LEAER_COUNT));
    }
    EXPECT_EQ(2, ObDITls<ObSessionDIBuffer>::get_instance()->get_tenant_id());
  }
  EXPECT_EQ(1, ObDITls<ObSessionDIBuffer>::get_instance()->get_tenant_id());
}

TEST(ObDISessionCache, multithread)
{
  ObDiagnoseSessionInfo info;
  ObDiagnoseTenantInfo tenant_info;
  ObWaitEventHistory& history = info.get_event_history();
  for (uint64_t i = 0; i < 2; i++) {
    info.notify_wait_begin(ObWaitEventIds::PT_LOCATION_CACHE_LOCK_WAIT, 0, 0, 0, 0);
    EXPECT_EQ(1, history.curr_pos_);
    EXPECT_EQ(1, history.item_cnt_);
    EXPECT_EQ(1, history.nest_cnt_);
    EXPECT_EQ(ObWaitEventIds::PT_LOCATION_CACHE_LOCK_WAIT,
        history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].event_no_);
    EXPECT_EQ(0, history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].level_);
    this_routine::usleep(1);
    info.notify_wait_end(&tenant_info);
    EXPECT_EQ(1, history.curr_pos_);
    EXPECT_EQ(1, history.item_cnt_);
    EXPECT_EQ(0, history.nest_cnt_);
    EXPECT_EQ(ObWaitEventIds::PT_LOCATION_CACHE_LOCK_WAIT,
        history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].event_no_);
    EXPECT_EQ(0, history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].level_);
    info.notify_wait_begin(ObWaitEventIds::MT_READ_LOCK_WAIT, 0, 0, 0, 0);
    EXPECT_EQ(2, history.curr_pos_);
    EXPECT_EQ(2, history.item_cnt_);
    EXPECT_EQ(1, history.nest_cnt_);
    EXPECT_EQ(ObWaitEventIds::MT_READ_LOCK_WAIT,
        history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].event_no_);
    EXPECT_EQ(0, history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].level_);
    info.notify_wait_begin(ObWaitEventIds::MT_READ_LOCK_WAIT, 0, 0, 0, 0);
    EXPECT_EQ(3, history.curr_pos_);
    EXPECT_EQ(3, history.item_cnt_);
    EXPECT_EQ(2, history.nest_cnt_);
    EXPECT_EQ(ObWaitEventIds::MT_READ_LOCK_WAIT,
        history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].event_no_);
    EXPECT_EQ(1, history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].level_);
    info.notify_wait_end(&tenant_info);
    EXPECT_EQ(3, history.curr_pos_);
    EXPECT_EQ(3, history.item_cnt_);
    EXPECT_EQ(2, history.nest_cnt_);
    EXPECT_EQ(ObWaitEventIds::MT_READ_LOCK_WAIT,
        history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].event_no_);
    EXPECT_EQ(0, history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].level_);
    info.notify_wait_begin(ObWaitEventIds::MT_READ_LOCK_WAIT, 0, 0, 0, 0);
    EXPECT_EQ(4, history.curr_pos_);
    EXPECT_EQ(4, history.item_cnt_);
    EXPECT_EQ(3, history.nest_cnt_);
    EXPECT_EQ(ObWaitEventIds::MT_READ_LOCK_WAIT,
        history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].event_no_);
    EXPECT_EQ(1, history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].level_);
    info.notify_wait_end(&tenant_info);
    info.notify_wait_begin(ObWaitEventIds::MT_READ_LOCK_WAIT, 0, 0, 0, 0);
    info.notify_wait_end(&tenant_info);
    info.notify_wait_begin(ObWaitEventIds::MT_READ_LOCK_WAIT, 0, 0, 0, 0);
    info.notify_wait_begin(ObWaitEventIds::MT_READ_LOCK_WAIT, 0, 0, 0, 0);
    EXPECT_EQ(7, history.curr_pos_);
    EXPECT_EQ(7, history.item_cnt_);
    EXPECT_EQ(6, history.nest_cnt_);
    EXPECT_EQ(ObWaitEventIds::MT_READ_LOCK_WAIT,
        history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].event_no_);
    EXPECT_EQ(2, history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].level_);
    info.notify_wait_begin(ObWaitEventIds::MT_READ_LOCK_WAIT, 0, 0, 0, 0);
    EXPECT_EQ(8, history.curr_pos_);
    EXPECT_EQ(8, history.item_cnt_);
    EXPECT_EQ(7, history.nest_cnt_);
    EXPECT_EQ(ObWaitEventIds::MT_READ_LOCK_WAIT,
        history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].event_no_);
    EXPECT_EQ(3, history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].level_);
    info.notify_wait_end(&tenant_info);
    info.notify_wait_end(&tenant_info);
    EXPECT_EQ(8, history.curr_pos_);
    EXPECT_EQ(8, history.item_cnt_);
    EXPECT_EQ(7, history.nest_cnt_);
    EXPECT_EQ(ObWaitEventIds::MT_READ_LOCK_WAIT,
        history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].event_no_);
    EXPECT_EQ(1, history.items_[(history.current_wait_ + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].level_);
    info.notify_wait_end(&tenant_info);
    if (1 == i) {
      info.notify_wait_begin(ObWaitEventIds::PT_LOCATION_CACHE_LOCK_WAIT, 0, 0, 0, 0);
      this_routine::usleep(1);
      info.notify_wait_end(&tenant_info);
      info.notify_wait_begin(ObWaitEventIds::PT_LOCATION_CACHE_LOCK_WAIT, 0, 0, 0, 0);
      this_routine::usleep(1);
      info.notify_wait_end(&tenant_info);
      info.notify_wait_end(&tenant_info);
      EXPECT_EQ(2, history.curr_pos_);
      EXPECT_EQ(2, history.item_cnt_);
      EXPECT_EQ(0, history.nest_cnt_);
      EXPECT_EQ(ObWaitEventIds::MT_READ_LOCK_WAIT,
          history.items_[(history.curr_pos_ - 1 + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].event_no_);
      EXPECT_EQ(
          0, history.items_[(history.curr_pos_ - 1 + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].level_);
      EXPECT_EQ(history.items_[8].wait_begin_time_,
          history.items_[(history.curr_pos_ - 1 + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT]
              .wait_begin_time_);
      EXPECT_EQ(history.items_[9].wait_end_time_,
          history.items_[(history.curr_pos_ - 1 + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].wait_end_time_);
    } else {
      info.notify_wait_end(&tenant_info);
      EXPECT_EQ(1, history.curr_pos_);
      EXPECT_EQ(1, history.item_cnt_);
      EXPECT_EQ(0, history.nest_cnt_);
      EXPECT_EQ(ObWaitEventIds::PT_LOCATION_CACHE_LOCK_WAIT,
          history.items_[(history.curr_pos_ - 1 + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].event_no_);
      EXPECT_EQ(
          0, history.items_[(history.curr_pos_ - 1 + SESSION_WAIT_HISTORY_CNT) % SESSION_WAIT_HISTORY_CNT].level_);
    }
    info.reset();
  }
}

TEST(ObDiagnoseSessionInfo, normal)
{
  ObWaitEventDesc max_wait;
  ObWaitEventStat total_wait;
  ObWaitEventDesc max_wait1;
  ObWaitEventStat total_wait1;
  {
    ObSessionStatEstGuard session_guard(1, 1);
    {
      ObMaxWaitGuard max_guard(&max_wait);
      ObTotalWaitGuard total_guard(&total_wait);
      for (int i = 0; i < 30; i++) {
        {
          ObWaitEventGuard wait_guard(i);
          this_routine::usleep(i);
        }
        ObMaxWaitGuard max_guard1(&max_wait1);
        ObTotalWaitGuard total_guard1(&total_wait1);
        {
          ObWaitEventGuard wait_guard1(29 - i);
          this_routine::usleep(i * 200);
        }
      }
      ObDiagnoseSessionInfo* info = ObDiagnoseSessionInfo::get_local_diagnose_info();
      EXPECT_EQ(1, info->get_tenant_id());
      ObWaitEventDesc* last_wait = NULL;
      info->get_event_history().get_last_wait(last_wait);
      EXPECT_EQ(0, last_wait->event_no_);
      EXPECT_EQ(0, info->get_curr_wait().event_no_);
      // EXPECT_EQ(0, max_wait.event_no_);
      EXPECT_EQ(54, total_wait.total_waits_);
      uint64_t begin_time = ::oceanbase::common::ObTimeUtility::current_time();
      for (int i = 0; i < 1000000; i++) {
        ObWaitEventGuard wait_guard(ObWaitEventIds::MT_READ_LOCK_WAIT);
        ObWaitEventGuard wait_guard1(ObWaitEventIds::MT_READ_LOCK_WAIT);
      }
      uint64_t end_time = ::oceanbase::common::ObTimeUtility::current_time();
      printf("scan sysstat time: %ld us\n", (end_time - begin_time));
      begin_time = ::oceanbase::common::ObTimeUtility::current_time();
      for (int i = 0; i < 1000000; i++) {
        ObWaitEventGuard wait_guard(ObWaitEventIds::MT_READ_LOCK_WAIT);
        ObWaitEventGuard wait_guard1(ObWaitEventIds::PT_LOCATION_CACHE_LOCK_WAIT);
      }
      end_time = ::oceanbase::common::ObTimeUtility::current_time();
      printf("scan sysstat time: %ld us\n", (end_time - begin_time));
    }
  }
}

class AtomicWaitEventTestRun : public cotesting::DefaultRunnable {
public:
  AtomicWaitEventTestRun()
  {}
  ~AtomicWaitEventTestRun()
  {}
  void run1() override
  {
    ObSessionStatEstGuard session_guard(1, 1);
    lock_.wrlock();
    usleep(10000);  // 10ms
    lock_.unlock();
  }

private:
  SpinRWLock lock_;
};

TEST(ObDiagnoseSessionInfo, atomic_wait_event)
{
  ObSessionStatEstGuard session_guard(1, 1);
  AtomicWaitEventTestRun test_run;
  test_run.set_thread_count(2);
  test_run.start();
  test_run.wait();
  ObDiagnoseSessionInfo* info = ObDiagnoseSessionInfo::get_local_diagnose_info();

  ASSERT_TRUE(NULL != info);
  ASSERT_EQ(1, info->get_tenant_id());

  ObWaitEventStat* event_stat = info->get_event_stats().get(ObWaitEventIds::LATCH_WAIT_QUEUE_LOCK_WAIT);
  ASSERT_TRUE(NULL != event_stat);
  EXPECT_TRUE(0 < event_stat->total_waits_);

  event_stat = info->get_event_stats().get(ObWaitEventIds::DEFAULT_SPIN_RWLOCK_WAIT);
  ASSERT_TRUE(NULL != event_stat);
  EXPECT_TRUE(0 < event_stat->total_waits_);
}

}  // namespace common
}  // namespace oceanbase

int main(int argc, char** argv)
{
  oceanbase::common::ObLogger::get_logger().set_log_level("INFO");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
