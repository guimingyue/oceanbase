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

#ifndef OCEANBASE_BASIC_OB_GROUPBY_OP_H_
#define OCEANBASE_BASIC_OB_GROUPBY_OP_H_

#include "sql/engine/ob_operator.h"
#include "sql/engine/aggregate/ob_aggregate_processor.h"

namespace oceanbase {
namespace sql {

// constant
class ObGroupBySpec : public ObOpSpec {
public:
  OB_UNIS_VERSION_V(1);

public:
  ObGroupBySpec(common::ObIAllocator& alloc, const ObPhyOperatorType type) : ObOpSpec(alloc, type), aggr_infos_(alloc)
  {}
  DECLARE_VIRTUAL_TO_STRING;

  //  int add_udf_meta(ObAggUDFDatumMeta &meta) { return agg_udf_meta_.push_back(meta); }

private:
  // disallow copy
  DISALLOW_COPY_AND_ASSIGN(ObGroupBySpec);

public:
  AggrInfoFixedArray aggr_infos_;  // aggr column + non-aggr column
  //  common::ObSEArray<ObAggUdfMeta, 16> agg_udf_meta_;
};

// modifiable
class ObGroupByOp : public ObOperator {
public:
  ObGroupByOp(ObExecContext& exec_ctx, const ObOpSpec& spec, ObOpInput* input)
      : ObOperator(exec_ctx, spec, input),
        aggr_processor_(eval_ctx_, (static_cast<ObGroupBySpec&>(const_cast<ObOpSpec&>(spec))).aggr_infos_)
  {}
  inline ObAggregateProcessor& get_aggr_processor()
  {
    return aggr_processor_;
  }

  virtual int inner_open() override;
  virtual int inner_get_next_row() override = 0;
  virtual int rescan() override;
  virtual int switch_iterator() override;
  virtual int inner_close() override;
  virtual void destroy() override;

private:
  void reset_default();
  // disallow copy
  DISALLOW_COPY_AND_ASSIGN(ObGroupByOp);

protected:
  ObAggregateProcessor aggr_processor_;
};

}  // end namespace sql
}  // end namespace oceanbase

#endif  // OCEANBASE_BASIC_OB_GROUPBY_OP_H_
