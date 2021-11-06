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

#ifndef _OB_TABLE_SERVICE_H
#define _OB_TABLE_SERVICE_H 1
#include "share/table/ob_table.h"
#include "observer/ob_server_struct.h"
#include "sql/engine/expr/ob_expr_res_type.h"
#include "share/table/ob_table_rpc_struct.h"
#include "storage/ob_dml_param.h"
#include "share/schema/ob_table_param.h"
namespace oceanbase
{
namespace table
{
class ObHTableFilterOperator;
class ObHColumnDescriptor;
} // end namespace table
namespace storage
{
class ObPartitionService;
}

namespace observer
{
using table::ObTableOperation;
using table::ObTableOperationResult;
using table::ObTableBatchOperation;
using table::ObTableBatchOperationResult;
using table::ObITableBatchOperationResult;
using table::ObTableQuery;
using table::ObTableQueryResult;
class ObTableApiProcessorBase;
class ObTableService;
class ObTableApiRowIterator;

class ObTableServiceCtx
{
  static const int64_t COMMON_COLUMN_NUM = 16;
public:
  common::ObSEArray<sql::ObExprResType, COMMON_COLUMN_NUM> columns_type_;
protected:
  friend class ObTableService;
  friend class ObTableApiRowIterator;
  struct Param
  {
    uint64_t table_id_;
    uint64_t partition_id_;
    int64_t timeout_ts_;
    ObTableApiProcessorBase *processor_;
    common::ObArenaAllocator *allocator_;
    bool returning_affected_rows_;
    table::ObTableEntityType entity_type_;
    table::ObBinlogRowImageType binlog_row_image_type_;
    bool returning_affected_entity_;
    bool returning_rowkey_;
    Param()
        :table_id_(common::OB_INVALID_ID),
         partition_id_(common::OB_INVALID_ID),
         timeout_ts_(0),
         processor_(nullptr),
         allocator_(nullptr),
         returning_affected_rows_(false),
         entity_type_(table::ObTableEntityType::ET_DYNAMIC),
         binlog_row_image_type_(table::ObBinlogRowImageType::FULL),
         returning_affected_entity_(false),
         returning_rowkey_(false)
    {}
  } param_;
public:
  ObTableServiceCtx()
      :param_()
  {}
  void reset_dml()
  {
    columns_type_.reset();
  }
  void init_param(int64_t timeout_ts,
                  ObTableApiProcessorBase *processor,
                  common::ObArenaAllocator *allocator,
                  bool returning_affected_rows,
                  table::ObTableEntityType entity_type,
                  table::ObBinlogRowImageType binlog_row_image_type,
                  bool returning_affected_entity = false,
                  bool returning_rowkey = false)
  {
    param_.timeout_ts_ = timeout_ts;
    param_.processor_ = processor;
    param_.allocator_ = allocator;
    param_.returning_affected_rows_ = returning_affected_rows;
    param_.entity_type_ = entity_type;
    param_.binlog_row_image_type_ = binlog_row_image_type;
    param_.returning_affected_entity_ = returning_affected_entity;
    param_.returning_rowkey_ = returning_rowkey;
  }
  uint64_t &param_table_id() { return param_.table_id_; }
  uint64_t &param_partition_id() { return param_.partition_id_; }
};

class ObTableServiceGetCtx: public ObTableServiceCtx
{
public:
  share::schema::ObTableParam table_param_on_stack_;
  share::schema::ObTableParam *table_param_;
  storage::ObTableScanParam scan_param_;
  common::ObNewRowIterator *scan_result_;
public:
  ObTableServiceGetCtx(common::ObArenaAllocator &alloc);
  void reset_get_ctx()
  {
    ObTableServiceCtx::reset_dml();
    table_param_->reset();
  }
};

class ObNormalTableQueryResultIterator: public table::ObTableQueryResultIterator
{
public:
  ObNormalTableQueryResultIterator(const ObTableQuery &query, table::ObTableQueryResult &one_result)
      :one_result_(one_result),
       query_(&query),
       last_row_(NULL),
       batch_size_(query.get_batch()),
       max_result_size_(std::min(query.get_max_result_size(),
                                 static_cast<int64_t>(common::OB_MAX_PACKET_BUFFER_LENGTH-1024))),
       scan_result_(NULL),
       is_first_result_(true),
       has_more_rows_(true)
  {
  }
  virtual ~ObNormalTableQueryResultIterator() {}
  virtual int get_next_result(table::ObTableQueryResult *&one_result) override;
  virtual bool has_more_result() const override;
  void set_scan_result(common::ObNewRowIterator *scan_result) { scan_result_ = scan_result; }
private:
  table::ObTableQueryResult &one_result_;
  const ObTableQuery *query_;
  common::ObNewRow *last_row_;
  int32_t batch_size_;
  int64_t max_result_size_;
  common::ObNewRowIterator *scan_result_;
  bool is_first_result_;
  bool has_more_rows_;
};

struct ObTableServiceQueryCtx: public ObTableServiceGetCtx
{
public:
  ObNormalTableQueryResultIterator *normal_result_iterator_;
  table::ObHTableFilterOperator *htable_result_iterator_;
public:
  ObTableServiceQueryCtx(common::ObArenaAllocator &alloc)
      :ObTableServiceGetCtx(alloc),
       normal_result_iterator_(NULL),
       htable_result_iterator_(NULL)
  {}
  void reset_query_ctx(storage::ObPartitionService *part_service)
  {
    destroy_result_iterator(part_service);
    ObTableServiceGetCtx::reset_get_ctx();
  }
  ObNormalTableQueryResultIterator *get_normal_result_iterator(const ObTableQuery &query,
                                                               table::ObTableQueryResult &one_result);
  table::ObHTableFilterOperator *get_htable_result_iterator(const ObTableQuery &query,
                                                            table::ObTableQueryResult &one_result);
  void destroy_result_iterator(storage::ObPartitionService *part_service);
};

/// table service
class ObTableService
{
public:
  friend class TestBatchExecute_obj_increment_Test;
public:
  ObTableService()
      :part_service_(NULL),
       schema_service_(NULL)
  {}
  virtual ~ObTableService() = default;
  int init(ObGlobalContext &gctx);

  int execute_get(ObTableServiceGetCtx &ctx, const ObTableOperation &table_operation, ObTableOperationResult &result);
  int execute_insert_or_update(ObTableServiceGetCtx &ctx, const ObTableOperation &table_operation, ObTableOperationResult &result);
  int execute_delete(ObTableServiceGetCtx &ctx, const ObTableOperation &table_operation, ObTableOperationResult &result);
  int execute_insert(ObTableServiceCtx &ctx, const ObTableOperation &table_operation,
      ObTableOperationResult &result, ObNewRowIterator *&duplicate_row_iter);
  int execute_replace(ObTableServiceCtx &ctx, const ObTableOperation &table_operation, ObTableOperationResult &result);
  int execute_update(ObTableServiceGetCtx &ctx, const ObTableOperation &table_operation,
      ObNewRow *target_row, ObTableOperationResult &result);
  int execute_increment(ObTableServiceGetCtx &ctx, const ObTableOperation &table_operation, ObTableOperationResult &result);

  int multi_get(ObTableServiceGetCtx &ctx, const ObTableBatchOperation &batch_operation, ObTableBatchOperationResult &result);
  int multi_insert_or_update(ObTableServiceGetCtx &ctx, const ObTableBatchOperation &batch_operation, ObTableBatchOperationResult &result);
  int multi_delete(ObTableServiceGetCtx &ctx, const ObTableBatchOperation &batch_operation, ObTableBatchOperationResult &result);
  int multi_insert(ObTableServiceCtx &ctx, const ObTableBatchOperation &batch_operation, ObTableBatchOperationResult &result);
  int multi_replace(ObTableServiceCtx &ctx, const ObTableBatchOperation &batch_operation, ObTableBatchOperationResult &result);
  int multi_update(ObTableServiceGetCtx &ctx, const ObTableBatchOperation &batch_operation, ObTableBatchOperationResult &result);

  int batch_execute(ObTableServiceGetCtx &ctx, const ObTableBatchOperation &batch_operation, ObTableBatchOperationResult &result);
  int execute_query(ObTableServiceQueryCtx &ctx, const ObTableQuery &query,
                    table::ObTableQueryResult &one_result, table::ObTableQueryResultIterator *&query_result);
private:
  static int cons_rowkey_infos(const share::schema::ObTableSchema &table_schema,
                               common::ObIArray<uint64_t> *column_ids,
                               common::ObIArray<sql::ObExprResType> *columns_type);
  static int cons_properties_infos(const share::schema::ObTableSchema &table_schema,
                                   const common::ObIArray<common::ObString> &properties,
                                   common::ObIArray<uint64_t> &column_ids,
                                   common::ObIArray<sql::ObExprResType> *columns_type);
  static int cons_column_type(const share::schema::ObColumnSchemaV2 &column_schema, sql::ObExprResType &column_type);
  static int check_column_type(const sql::ObExprResType &column_type, common::ObObj &obj);
  static int add_index_columns_if_missing(share::schema::ObSchemaGetterGuard &schema_guard,
                                          uint64_t data_table_id,
                                          const share::schema::ObTableSchema *index_schema,
                                          common::ObIArray<uint64_t> &column_ids,
                                          common::ObIArray<sql::ObExprResType> *columns_type);

  int insert_or_update_can_use_put(table::ObTableEntityType entity_type, uint64_t table_id, const table::ObITableEntity &entity, bool &use_put);
  int add_one_result(ObTableBatchOperationResult &result,
                     table::ObTableOperationType::Type op_type,
                     int32_t error_code,
                     int64_t affected_rows);
  int do_put(ObTableServiceCtx &ctx, const ObTableOperation &table_operation, ObTableOperationResult &result);
  int do_insert_or_update(ObTableServiceGetCtx &ctx, const ObTableOperation &table_operation, ObTableOperationResult &result);
  int multi_put(ObTableServiceCtx &ctx, const ObTableBatchOperation &batch_operation, ObTableBatchOperationResult &result);
  int do_multi_insert_or_update(ObTableServiceGetCtx &ctx,
                                const ObTableBatchOperation &batch_operation,
                                ObTableBatchOperationResult &result);
  // for get
  int fill_scan_param(ObTableServiceCtx &ctx,
                      const ObIArray<uint64_t> &output_column_ids,
                      int64_t schema_version,
                      storage::ObTableScanParam &scan_param);
  int fill_get_result(
      ObTableServiceCtx &ctx,
      const ObIArray<ObString> &properties,
      ObTableApiRowIterator *scan_result,
      ObTableOperationResult &operation_result);
  // for multi-get
  int fill_multi_get_result(
      ObTableServiceGetCtx &ctx,
      const ObTableBatchOperation &batch_operation,
      ObTableApiRowIterator *scan_result,
      ObTableBatchOperationResult &result);
  int delete_can_use_put(table::ObTableEntityType entity_type, uint64_t table_id, bool &use_put);
  static int cons_all_index_properties(share::schema::ObSchemaGetterGuard &schema_guard,
                                       const share::schema::ObTableSchema &table_schema,
                                       common::ObIArray<uint64_t> &column_ids,
                                       common::ObIArray<sql::ObExprResType> *columns_type);

  // for replace
  int do_replace(
      ObTableServiceCtx &ctx,
      common::ObPartitionKey &part_key,
      storage::ObDMLBaseParam &dml_param,
      common::ObIArray<uint64_t> &column_ids,
      common::ObIArray<uint64_t> &rowkey_column_ids,
      common::ObNewRow &row,
      int64_t &affected_rows);
  // for replace delete duplicate row
  int do_replace_delete(
      ObTableServiceCtx &ctx,
      common::ObPartitionKey &part_key,
      storage::ObDMLBaseParam &dml_param,
      common::ObIArray<uint64_t> &column_ids,
      common::ObNewRowIterator *duplicated_rows,
      int64_t &affected_rows);
  // for execute_query
  int cons_index_key_type(share::schema::ObSchemaGetterGuard &schema_guard,
                          const share::schema::ObTableSchema *index_schema,
                          uint64_t data_table_id,
                          common::ObIArray<sql::ObExprResType> &columns_type);
  int get_index_id_by_name(share::schema::ObSchemaGetterGuard &schema_guard, uint64_t base_table_id,
                           const ObString &index_name, uint64_t &index_id,
                           common::ObIArray<sql::ObExprResType> &columns_type,
                           const share::schema::ObTableSchema *&index_schema);
  int fill_query_table_param(uint64_t table_id,
                             const common::ObIArray<ObString> &properties,
                             const ObString &index_name,
                             share::schema::ObTableParam &table_param,
                             common::ObIArray<uint64_t> &output_column_ids,
                             common::ObIArray<sql::ObExprResType> &rowkey_columns_type,
                             int64_t &schema_version,
                             uint64_t &index_id,
                             int64_t &padding_num,
                             table::ObHColumnDescriptor *hcolumn_desc);
  int fill_query_scan_ranges(ObTableServiceCtx &ctx,
                             const ObTableQuery &query,
                             int64_t padding_num,
                             storage::ObTableScanParam &scan_param);
  int fill_query_scan_param(ObTableServiceCtx &ctx,
                            const common::ObIArray<uint64_t> &output_column_ids,
                            int64_t schema_version,
                            ObQueryFlag::ScanOrder scan_order,
                            uint64_t index_id,
                            int32_t limit,
                            int32_t offset,
                            storage::ObTableScanParam &scan_param);
  int check_htable_query_args(const ObTableQuery &query);
private:
  int fill_new_entity(
      bool returning_rowkey,
      const common::ObNewRow &row,
      const int64_t primary_key_size,
      const common::ObIArray<common::ObString> &properties,
      common::ObIAllocator &alloc,
      table::ObITableEntity *new_entity);
  int execute_increment_by_update(ObTableServiceGetCtx &ctx,
                                  const ObTableOperation &table_operation,
                                  ObTableOperationResult &result);
private:
  // disallow copy
  DISALLOW_COPY_AND_ASSIGN(ObTableService);
private:
  static const int64_t COMMON_COLUMN_NUM = 16;
  storage::ObPartitionService *part_service_;
  share::schema::ObMultiVersionSchemaService *schema_service_;
};

} // end namespace observer
} // end namespace oceanbase

#endif /* _OB_TABLE_SERVICE_H */
