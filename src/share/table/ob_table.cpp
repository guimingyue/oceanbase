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

#define USING_LOG_PREFIX CLIENT
#include "ob_table.h"
#include "share/ob_errno.h"
#include "lib/utility/ob_unify_serialize.h"
#include "common/row/ob_row.h"
#include "rpc/obrpc/ob_rpc_packet.h"

using namespace oceanbase::table;
using namespace oceanbase::common;

OB_DEF_SERIALIZE(ObITableEntity)
{
  int ret = OB_SUCCESS;
  if (OB_SUCC(ret)) {
    const int64_t rowkey_size = get_rowkey_size();
    OB_UNIS_ENCODE(rowkey_size);
    ObObj obj;
    for (int64_t i = 0; i < rowkey_size && OB_SUCCESS == ret; ++i) {
      if (OB_FAIL(this->get_rowkey_value(i, obj))) {
        LOG_WARN("failed to get value", K(ret), K(i));
      }
      OB_UNIS_ENCODE(obj);
    }
  }
  if (OB_SUCC(ret)) {
    ObSEArray<std::pair<ObString, ObObj>, 8> properties;
    if (OB_FAIL(this->get_properties(properties))) {  // @todo optimize, use iterator
      LOG_WARN("failed to get properties", K(ret));
    } else {
      const int64_t properties_count = properties.count();
      OB_UNIS_ENCODE(properties_count);
      for (int64_t i = 0; i < properties_count && OB_SUCCESS == ret; ++i) {
        const std::pair<ObString, ObObj> &kv_pair = properties.at(i);
        OB_UNIS_ENCODE(kv_pair.first);
        OB_UNIS_ENCODE(kv_pair.second);
      }
    }
  }
  return ret;
}

OB_DEF_DESERIALIZE(ObITableEntity)
{
  int ret = OB_SUCCESS;
  reset();
  ObString key;
  ObObj value;
  if (NULL == alloc_) {
    // shallow copy
    if (OB_SUCC(ret)) {
      int64_t rowkey_size = -1;
      OB_UNIS_DECODE(rowkey_size);
      for (int64_t i = 0; OB_SUCCESS == ret && i < rowkey_size; ++i) {
        OB_UNIS_DECODE(value);
        if (OB_SUCC(ret)) {
          if (OB_FAIL(this->add_rowkey_value(value))) {
            LOG_WARN("failed to add rowkey value", K(ret), K(value));
          }
        }
      }
    }
    if (OB_SUCC(ret)) {
      int64_t properties_count = -1;
      OB_UNIS_DECODE(properties_count);
      for (int64_t i = 0; i < properties_count && OB_SUCCESS == ret; ++i) {
        OB_UNIS_DECODE(key);
        OB_UNIS_DECODE(value);
        if (OB_SUCC(ret)) {
          if (OB_FAIL(this->set_property(key, value))) {
            LOG_WARN("failed to set property", K(ret), K(key), K(value));
          }
        }
      }
    }
  } else {
    // deep copy
    ObObj value_clone;
    if (OB_SUCC(ret)) {
      int64_t rowkey_size = -1;
      OB_UNIS_DECODE(rowkey_size);
      for (int64_t i = 0; OB_SUCCESS == ret && i < rowkey_size; ++i) {
        OB_UNIS_DECODE(value);
        if (OB_SUCC(ret)) {
          if (OB_FAIL(ob_write_obj(*alloc_, value, value_clone))) {
            LOG_WARN("failed to copy value", K(ret));
          } else if (OB_FAIL(this->add_rowkey_value(value_clone))) {
            LOG_WARN("failed to add rowkey value", K(ret), K(value_clone));
          }
        }
      }
    }
    if (OB_SUCC(ret)) {
      ObString key_clone;
      int64_t properties_count = -1;
      OB_UNIS_DECODE(properties_count);
      for (int64_t i = 0; i < properties_count && OB_SUCCESS == ret; ++i) {
        OB_UNIS_DECODE(key);
        OB_UNIS_DECODE(value);
        if (OB_SUCC(ret)) {
          if (OB_FAIL(ob_write_string(*alloc_, key, key_clone))) {
            LOG_WARN("failed to clone string", K(ret));
          } else if (OB_FAIL(ob_write_obj(*alloc_, value, value_clone))) {
            LOG_WARN("failed to copy value", K(ret));
          } else if (OB_FAIL(this->set_property(key_clone, value_clone))) {
            LOG_WARN("failed to set property", K(ret), K(key), K(value_clone));
          }
        }
      }
    }
  }
  return ret;
}

OB_DEF_SERIALIZE_SIZE(ObITableEntity)
{
  int64_t len = 0;
  int ret = OB_SUCCESS;
  ObString key;
  ObObj value;
  const int64_t rowkey_size = get_rowkey_size();
  OB_UNIS_ADD_LEN(rowkey_size);
  for (int64_t i = 0; i < rowkey_size && OB_SUCCESS == ret; ++i) {
    if (OB_FAIL(this->get_rowkey_value(i, value))) {
      LOG_WARN("failed to get value", K(ret), K(i));
    }
    OB_UNIS_ADD_LEN(value);
  }
  if (OB_SUCC(ret)) {
    ObSEArray<std::pair<ObString, ObObj>, 8> properties;
    if (OB_FAIL(this->get_properties(properties))) {  // @todo optimize, use iterator
      LOG_WARN("failed to get properties", K(ret));
    } else {
      const int64_t properties_count = properties.count();
      OB_UNIS_ADD_LEN(properties_count);
      for (int64_t i = 0; i < properties_count && OB_SUCCESS == ret; ++i) {
        const std::pair<ObString, ObObj> &kv_pair = properties.at(i);
        OB_UNIS_ADD_LEN(kv_pair.first);
        OB_UNIS_ADD_LEN(kv_pair.second);
      }
    }
  }
  return len;
}

int ObITableEntity::deep_copy(common::ObIAllocator &allocator, const ObITableEntity &other)
{
  int ret = OB_SUCCESS;
  reset();
  if (OB_FAIL(deep_copy_rowkey(allocator, other))) {
  } else if (OB_FAIL(deep_copy_properties(allocator, other))) {
  }
  return ret;
}

int ObITableEntity::deep_copy_rowkey(common::ObIAllocator &allocator, const ObITableEntity &other)
{
  int ret = OB_SUCCESS;
  ObObj value;
  ObObj cell_clone;
  const int64_t rowkey_size = other.get_rowkey_size();
  for (int64_t i = 0; OB_SUCCESS == ret && i < rowkey_size; ++i)
  {
    if (OB_FAIL(other.get_rowkey_value(i, value))) {
      LOG_WARN("failed to get rowkey value", K(ret), K(i), K(value));
    } else if (OB_FAIL(ob_write_obj(allocator, value, cell_clone))) {
      LOG_WARN("failed to copy cell", K(ret));
    } else if (OB_FAIL(this->add_rowkey_value(cell_clone))) {
      LOG_WARN("failed to add rowkey value", K(ret), K(value));
    }
  } // end for
  return ret;
}

int ObITableEntity::deep_copy_properties(common::ObIAllocator &allocator, const ObITableEntity &other)
{
  int ret = OB_SUCCESS;
  ObObj value;
  ObObj cell_clone;
  ObString name_clone;
  ObSEArray<std::pair<ObString, ObObj>, 8> properties;
  if (OB_FAIL(other.get_properties(properties))) {  // @todo optimize, use iterator
    LOG_WARN("failed to get properties", K(ret));
  } else {
    const int64_t properties_count = properties.count();
    for (int64_t i = 0; i < properties_count && OB_SUCCESS == ret; ++i) {
      const std::pair<ObString, ObObj> &kv_pair = properties.at(i);
      if (OB_FAIL(ob_write_string(allocator, kv_pair.first, name_clone))) {
        LOG_WARN("failed to clone string", K(ret));
      } else if (OB_FAIL(ob_write_obj(allocator, kv_pair.second, cell_clone))) {
        LOG_WARN("failed to copy cell", K(ret));
      } else if (OB_FAIL(this->set_property(name_clone, cell_clone))) {
        LOG_WARN("failed to set property", K(ret));
      }
    }  // end for
  }
  return ret;
}

int ObITableEntity::add_retrieve_property(const ObString &prop_name)
{
  ObObj null_obj;
  return set_property(prop_name, null_obj);
}

////////////////////////////////////////////////////////////////
ObTableEntity::ObTableEntity()
{}

ObTableEntity::~ObTableEntity()
{}

inline int ObTableEntity::try_init()
{
  int ret = OB_SUCCESS;
  static const int64_t PROPERTY_MAP_BUCKET_SIZE = 107;
  if (!properties_.created()) {
    if (OB_FAIL(properties_.create(PROPERTY_MAP_BUCKET_SIZE, ObModIds::OB_HASH_BUCKET_SQL_COLUMN_MAP,
                                   ObModIds::OB_HASH_NODE_SQL_COLUMN_MAP))) {
      LOG_WARN("failed to init properties", K(ret));
    }
  }
  return ret;
}

int ObTableEntity::set_rowkey_value(int64_t idx, const ObObj &value)
{
  int ret = OB_SUCCESS;
  if (idx < 0) {
    ret = OB_INDEX_OUT_OF_RANGE;
  } else if (idx < rowkey_.count()) {
    rowkey_.at(idx) = value;
  } else {
    int64_t N = rowkey_.count();
    ObObj null_obj;
    for (int64_t i = N; OB_SUCC(ret) && i < idx; ++i) {
      if (OB_FAIL(rowkey_.push_back(null_obj))) {
        LOG_WARN("failed to pad null obj", K(ret));
      }
    }
    if (OB_SUCC(ret)) {
      if (OB_FAIL(rowkey_.push_back(value))) {
        LOG_WARN("failed to add value obj", K(ret), K(value));
      }
    }
  }
  return ret;
}

int ObTableEntity::add_rowkey_value(const ObObj &value)
{
  return rowkey_.push_back(value);
}

int ObTableEntity::get_rowkey_value(int64_t idx, ObObj &value) const
{
  return rowkey_.at(idx, value);
}

int ObTableEntity::set_rowkey(const ObRowkey &rowkey)
{
  int ret = OB_SUCCESS;
  rowkey_.reset();
  const int64_t N = rowkey.get_obj_cnt();
  for (int64_t i = 0; OB_SUCCESS == ret && i < N; ++i)
  {
    if (OB_FAIL(rowkey_.push_back(rowkey.ptr()[i]))) {
      LOG_WARN("failed to push back rowkey", K(ret), K(i));
    }
  } // end for
  return ret;
}

int ObTableEntity::set_rowkey(const ObITableEntity &other)
{
  int ret = OB_SUCCESS;
  const ObTableEntity *other_entity = dynamic_cast<const ObTableEntity*>(&other);
  if (NULL == other_entity) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid type of other entity");
  } else {
    rowkey_.reset();
    int64_t N = other_entity->rowkey_.count();
    for (int64_t i = 0; OB_SUCCESS == ret && i < N; ++i)
    {
      if (OB_FAIL(rowkey_.push_back(other_entity->rowkey_.at(i)))) {
        LOG_WARN("failed to push back rowkey");
      }
    } // end for
  }
  return ret;
}

int64_t ObTableEntity::hash_rowkey() const
{
  int64_t hash_value = 0;
  const int64_t N = rowkey_.count();
  for (int64_t i = 0; i < N; ++i)
  {
    hash_value = rowkey_.at(i).hash(hash_value);
  } // end for
  return hash_value;
}

int ObTableEntity::get_property(const ObString &prop_name, ObObj &prop_value) const
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(const_cast<ObTableEntity*>(this)->try_init())) {
    LOG_WARN("failed to init hash map", K(ret));
  } else if (OB_FAIL(properties_.get_refactored(prop_name, prop_value))) {
    LOG_DEBUG("failed to get property", K(ret), K(prop_name));
  }
  return ret;
}

int ObTableEntity::set_property(const ObString &prop_name, const ObObj &prop_value)
{
  int ret = OB_SUCCESS;
  if (prop_name.empty()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("property name should not be empty string", K(ret), K(prop_name));
  } else if (OB_FAIL(try_init())) {
    LOG_WARN("failed to init hash map", K(ret));
  } else if (OB_FAIL(properties_.set_refactored(prop_name, prop_value, 1))) {
    LOG_WARN("failed to set property", K(ret), K(prop_name));
  }
  return ret;
}

class ObTableEntity::GetPropertyFn
{
public:
  GetPropertyFn(ObIArray<std::pair<ObString, ObObj> > &properties)
      :properties_(properties)
  {}
  bool operator()(const hash::HashMapPair<ObString, ObObj> &kv)
  {
    int ret = OB_SUCCESS;
    if (OB_FAIL(properties_.push_back(std::make_pair(kv.first, kv.second)))) {
      LOG_WARN("failed to push back", K(ret));
    }
    return OB_SUCCESS == ret;
  }
private:
  ObIArray<std::pair<ObString, ObObj> > &properties_;
};

int ObTableEntity::get_properties(ObIArray<std::pair<ObString, ObObj> > &properties) const
{
  int ret = OB_SUCCESS;
  GetPropertyFn func(properties);
  if (OB_FAIL(const_cast<ObTableEntity*>(this)->try_init())) {
    LOG_WARN("failed to init hash map", K(ret));
  } else if (OB_FAIL(const_cast<PropertiesMap&>(properties_).foreach_refactored(func))) {
    LOG_WARN("for each properties fail", K(ret));
  }
  return ret;
}

class ObTableEntity::GetPropertyNameFn
{
public:
  GetPropertyNameFn(ObIArray<ObString> &names)
      :names_(names)
  {}
  bool operator()(const hash::HashMapPair<ObString, ObObj> &kv)
  {
    int ret = OB_SUCCESS;
    if (OB_FAIL(names_.push_back(kv.first))) {
      LOG_WARN("failed to push back", K(ret));
    }
    return OB_SUCCESS == ret;
  }
private:
  ObIArray<ObString> &names_;
};

int ObTableEntity::get_properties_names(ObIArray<ObString> &properties_names) const
{
  int ret = OB_SUCCESS;
  GetPropertyNameFn func(properties_names);
  if (OB_FAIL(const_cast<ObTableEntity*>(this)->try_init())) {
    LOG_WARN("failed to init hash map", K(ret));
  } else if (OB_FAIL(const_cast<PropertiesMap&>(properties_).foreach_refactored(func))) {
    LOG_WARN("for each properties fail", K(ret));
  }
  return ret;
}

class ObTableEntity::GetPropertyValueFn
{
public:
  GetPropertyValueFn(ObIArray<ObObj> &values)
      :values_(values)
  {}
  bool operator()(const hash::HashMapPair<ObString, ObObj> &kv)
  {
    int ret = OB_SUCCESS;
    if (OB_FAIL(values_.push_back(kv.second))) {
      LOG_WARN("failed to push back", K(ret));
    }
    return OB_SUCCESS == ret;
  }
private:
  ObIArray<ObObj> &values_;
};

int ObTableEntity::get_properties_values(ObIArray<ObObj> &properties_values) const
{
  int ret = OB_SUCCESS;
  GetPropertyValueFn func(properties_values);
  if (OB_FAIL(const_cast<ObTableEntity*>(this)->try_init())) {
    LOG_WARN("failed to init hash map", K(ret));
  } else if (OB_FAIL(const_cast<PropertiesMap&>(properties_).foreach_refactored(func))) {
    LOG_WARN("for each properties fail", K(ret));
  }
  return ret;
}

int64_t ObTableEntity::get_properties_count() const
{
  return properties_.size();
}

ObRowkey ObTableEntity::get_rowkey()
{
  ObRowkey rowkey;
  int64_t obj_cnt = rowkey_.count();
  if (obj_cnt > 0) {
    rowkey.assign(&rowkey_.at(0), obj_cnt);
  }
  return rowkey;
}

DEF_TO_STRING(ObTableEntity)
{
  int64_t pos = 0;
  J_OBJ_START();

  J_NAME("rowkey");
  J_COLON();
  BUF_PRINTO(rowkey_);

  ObSEArray<std::pair<ObString, ObObj>, 8> properties;
  int ret = OB_SUCCESS;
  if (OB_FAIL(this->get_properties(properties))) {
    LOG_WARN("failed to get properties", K(ret));
  } else {
    J_COMMA();
    J_NAME("properties");
    J_COLON();
    J_OBJ_START();
    int64_t N = properties.count();
    for (int64_t i = 0; OB_SUCCESS == ret && i < N; ++i)
    {
      const std::pair<ObString, ObObj> &prop = properties.at(i);
      if (0 != i) {
        J_COMMA();
      }
      BUF_PRINTF("%.*s", prop.first.length(), prop.first.ptr());
      J_COLON();
      BUF_PRINTO(prop.second);
    }
    J_OBJ_END();
  }
  J_OBJ_END();
  return pos;
}

////////////////////////////////////////////////////////////////
ObTableOperation ObTableOperation::insert(const ObITableEntity &entity)
{
  ObTableOperation op;
  op.operation_type_ = ObTableOperationType::INSERT;
  op.entity_ = &entity;
  return op;
}

ObTableOperation ObTableOperation::del(const ObITableEntity &entity)
{
  ObTableOperation op;
  op.operation_type_ = ObTableOperationType::DEL;
  op.entity_ = &entity;
  return op;
}

ObTableOperation ObTableOperation::update(const ObITableEntity &entity)
{
  ObTableOperation op;
  op.operation_type_ = ObTableOperationType::UPDATE;
  op.entity_ = &entity;
  return op;
}

ObTableOperation ObTableOperation::insert_or_update(const ObITableEntity &entity)
{
  ObTableOperation op;
  op.operation_type_ = ObTableOperationType::INSERT_OR_UPDATE;
  op.entity_ = &entity;
  return op;
}

ObTableOperation ObTableOperation::replace(const ObITableEntity &entity)
{
  ObTableOperation op;
  op.operation_type_ = ObTableOperationType::REPLACE;
  op.entity_ = &entity;
  return op;
}

ObTableOperation ObTableOperation::retrieve(const ObITableEntity &entity)
{
  ObTableOperation op;
  op.operation_type_ = ObTableOperationType::GET;
  op.entity_ = &entity;
  return op;
}

ObTableOperation ObTableOperation::increment(const ObITableEntity &entity)
{
  ObTableOperation op;
  op.operation_type_ = ObTableOperationType::INCREMENT;
  op.entity_ = &entity;
  return op;
}

ObTableOperation ObTableOperation::append(const ObITableEntity &entity)
{
  ObTableOperation op;
  op.operation_type_ = ObTableOperationType::APPEND;
  op.entity_ = &entity;
  return op;
}

int ObTableOperation::get_entity(ObITableEntity *&entity)
{
  int ret = OB_SUCCESS;
  if (NULL == entity_) {
    ret = OB_ERR_NULL_VALUE;
  } else {
    entity = const_cast<ObITableEntity*>(this->entity_);
  }
  return ret;
}

uint64_t ObTableOperation::get_checksum()
{
  uint64_t checksum = 0;
  ObITableEntity *entity = NULL;
  if (OB_SUCCESS != get_entity(entity) || OB_ISNULL(entity)) {
    // ignore
  } else {
    const int64_t rowkey_size = entity->get_rowkey_size();
    const int64_t property_count = entity->get_properties_count();
    checksum = ob_crc64(checksum, &rowkey_size, sizeof(rowkey_size));
    checksum = ob_crc64(checksum, &property_count, sizeof(property_count));
  }
  checksum = ob_crc64(checksum, &operation_type_, sizeof(operation_type_));
  return checksum;
}

OB_SERIALIZE_MEMBER(ObTableOperation, operation_type_, const_cast<ObITableEntity&>(*entity_));
////////////////////////////////////////////////////////////////
ObTableRequestOptions::ObTableRequestOptions()
    :consistency_level_(ObTableConsistencyLevel::STRONG),
     server_timeout_us_(10*1000*1000),
     max_execution_time_us_(10*1000*1000),
     retry_policy_(NULL),
     returning_affected_rows_(false),
     returning_rowkey_(false),
     returning_affected_entity_(false),
     binlog_row_image_type_(ObBinlogRowImageType::FULL)
{}

////////////////////////////////////////////////////////////////
int ObTableBatchOperation::add(const ObTableOperation &table_operation)
{
  int ret = table_operations_.push_back(table_operation);
  if (OB_SUCC(ret)) {
    if (is_readonly_ && ObTableOperationType::GET != table_operation.type()) {
      is_readonly_ = false;
    }
    if (is_same_type_) {
      const int64_t N = table_operations_.count();
      if (N >= 2 && table_operations_.at(N-1).type() != table_operations_.at(N-2).type()) {
        is_same_type_ = false;
      }
    }
    if (is_same_properties_names_) {
      const int64_t num = table_operations_.count();
      if (num >= 2) {
        const ObTableOperation &prev = table_operations_.at(num-2);
        const ObTableOperation &curr = table_operations_.at(num-1);
        ObSEArray<ObString, 8> prev_columns;
        ObSEArray<ObString, 8> curr_columns;
        if (OB_FAIL(prev.entity().get_properties_names(prev_columns))) {
          LOG_WARN("failed to get retrieve columns", K(ret));
        } else if (OB_FAIL(curr.entity().get_properties_names(curr_columns))) {
          LOG_WARN("failed to get retrieve columns", K(ret));
        } else if (prev_columns.count() != curr_columns.count()) {
          is_same_properties_names_ = false;
        } else if (prev_columns.count() == 1) {
          is_same_properties_names_ = (prev_columns.at(0) == curr_columns.at(0));
        } else {
          int64_t N = prev_columns.count();
          ObObj value;
          for (int64_t i = 0; OB_SUCCESS == ret && i < N && is_same_properties_names_; ++i)
          {
            const ObString &name = prev_columns.at(i);
            if (OB_FAIL(curr.entity().get_property(name, value))) {
              if (OB_HASH_NOT_EXIST == ret) {
                is_same_properties_names_ = false;
              }
            }
          } // end for
        }
      }
    }
  }
  return ret;
}

int ObTableBatchOperation::insert(const ObITableEntity &entity)
{
  ObTableOperation op = ObTableOperation::insert(entity);
  return add(op);
}

int ObTableBatchOperation::del(const ObITableEntity &entity)
{
  ObTableOperation op = ObTableOperation::del(entity);
  return add(op);
}

int ObTableBatchOperation::update(const ObITableEntity &entity)
{
  ObTableOperation op = ObTableOperation::update(entity);
  return add(op);
}

int ObTableBatchOperation::insert_or_update(const ObITableEntity &entity)
{
  ObTableOperation op = ObTableOperation::insert_or_update(entity);
  return add(op);
}

int ObTableBatchOperation::replace(const ObITableEntity &entity)
{
  ObTableOperation op = ObTableOperation::replace(entity);
  return add(op);
}

int ObTableBatchOperation::retrieve(const ObITableEntity &entity)
{
  ObTableOperation op = ObTableOperation::retrieve(entity);
  return add(op);
}

int ObTableBatchOperation::increment(const ObITableEntity &entity)
{
  ObTableOperation op = ObTableOperation::increment(entity);
  return add(op);
}

int ObTableBatchOperation::append(const ObITableEntity &entity)
{
  ObTableOperation op = ObTableOperation::append(entity);
  return add(op);
}

uint64_t ObTableBatchOperation::get_checksum()
{
  uint64_t checksum = 0;
  const int64_t op_count = table_operations_.count();
  if (op_count > 0) {
    if (is_same_type()) {
      const uint64_t first_op_checksum = table_operations_.at(0).get_checksum();
      checksum = ob_crc64(checksum, &first_op_checksum, sizeof(first_op_checksum));
    } else {
      for (int64_t i = 0; i < op_count; ++i) {
        const uint64_t cur_op_checksum = table_operations_.at(i).get_checksum();
        checksum = ob_crc64(checksum, &cur_op_checksum, sizeof(cur_op_checksum));
      }
    }
  }
  checksum = ob_crc64(checksum, &is_readonly_, sizeof(is_readonly_));
  checksum = ob_crc64(checksum, &is_same_type_, sizeof(is_same_type_));
  checksum = ob_crc64(checksum, &is_same_properties_names_, sizeof(is_same_properties_names_));

  return checksum;
}

void ObTableBatchOperation::reset()
{
  table_operations_.reset();
  is_readonly_ = true;
  is_same_type_ = true;
  is_same_properties_names_ = true;
}
/*
OB_SERIALIZE_MEMBER(ObTableBatchOperation,
                    table_operations_,
                    is_readonly_,
                    is_same_type_,
                    is_same_properties_names_);
*/
OB_UNIS_DEF_SERIALIZE(ObTableBatchOperation,
                      table_operations_,
                      is_readonly_,
                      is_same_type_,
                      is_same_properties_names_);

OB_UNIS_DEF_SERIALIZE_SIZE(ObTableBatchOperation,
                           table_operations_,
                           is_readonly_,
                           is_same_type_,
                           is_same_properties_names_);

OB_DEF_DESERIALIZE(ObTableBatchOperation,)
{
  int ret = OB_SUCCESS;
  UNF_UNUSED_DES;
  reset();
  int64_t batch_size = 0;
  OB_UNIS_DECODE(batch_size);
  ObITableEntity *entity = NULL;
  ObTableOperation table_operation;
  for (int64_t i = 0; OB_SUCCESS == ret && i < batch_size; ++i)
  {
    if (NULL == (entity = entity_factory_->alloc())) {
      ret = OB_ALLOCATE_MEMORY_FAILED;
    } else {
      table_operation.set_entity(*entity);
      OB_UNIS_DECODE(table_operation);
      if (OB_SUCC(ret)) {
        if (OB_FAIL(table_operations_.push_back(table_operation))) {
          LOG_WARN("failed to push back", K(ret));
        }
      }
    }
  } // end for
  LST_DO_CODE(OB_UNIS_DECODE,
              is_readonly_,
              is_same_type_,
              is_same_properties_names_);
  return ret;
}

////////////////////////////////////////////////////////////////
OB_SERIALIZE_MEMBER(ObTableResult, errno_, sqlstate_, msg_);

int ObTableResult::assign(const ObTableResult &other)
{
  errno_ = other.errno_;
  strncpy(sqlstate_, other.sqlstate_, sizeof(sqlstate_));
  strncpy(msg_, other.msg_, sizeof(msg_));
  return OB_SUCCESS;
}

////////////////////////////////////////////////////////////////
ObTableOperationResult::ObTableOperationResult()
    :operation_type_(ObTableOperationType::GET),
     entity_(NULL),
     affected_rows_(0)
{}

int ObTableOperationResult::get_entity(const ObITableEntity *&entity) const
{
  int ret = OB_SUCCESS;
  if (NULL == entity_) {
    ret = OB_ERR_NULL_VALUE;
  } else {
    entity = entity_;
  }
  return ret;
}

int ObTableOperationResult::get_entity(ObITableEntity *&entity)
{
  int ret = OB_SUCCESS;
  if (NULL == entity_) {
    ret = OB_ERR_NULL_VALUE;
  } else {
    entity = entity_;
  }
  return ret;
}

DEF_TO_STRING(ObTableOperationResult)
{
  int64_t pos = 0;
  J_OBJ_START();
  J_KV(K_(errno),
       K_(operation_type),
       K_(affected_rows));
  J_COMMA();
  if (NULL == entity_) {
    J_NAME("entity");
    J_COLON();
    J_NULL();
  } else {
    J_KV("entity", *entity_);
  }
  J_OBJ_END();
  return pos;
}

OB_SERIALIZE_MEMBER((ObTableOperationResult, ObTableResult),
                    operation_type_, *entity_, affected_rows_);

int ObTableOperationResult::deep_copy(common::ObIAllocator &allocator,
                                      ObITableEntityFactory &entity_factory, const ObTableOperationResult &other)
{
  int ret = OB_SUCCESS;
  const ObITableEntity *src_entity = NULL;
  ObITableEntity *dest_entity = NULL;
  if (OB_FAIL(other.get_entity(src_entity))) {
    LOG_WARN("failed to get entity", K(ret));
  } else if (NULL == (dest_entity = entity_factory.alloc())) {
    ret = OB_ALLOCATE_MEMORY_FAILED;
    LOG_WARN("no memory", K(ret));
  } else if (OB_FAIL(dest_entity->deep_copy(allocator, *src_entity))) {
    LOG_WARN("failed to copy entity", K(ret));
  } else if (OB_FAIL(ObTableResult::assign(other))) {
    LOG_WARN("failed to copy result", K(ret));
  } else {
    operation_type_ = other.operation_type_;
    entity_ = dest_entity;
    affected_rows_ = other.affected_rows_;
  }
  return ret;
}

////////////////////////////////////////////////////////////////
OB_DEF_DESERIALIZE(ObTableBatchOperationResult,)
{
  int ret = OB_SUCCESS;
  UNF_UNUSED_DES;
  int64_t batch_size = 0;
  OB_UNIS_DECODE(batch_size);
  ObITableEntity *entity = NULL;
  ObTableOperationResult table_operation_result;
  reset();
  if (NULL == alloc_) {
    // shallow copy properties
    for (int64_t i = 0; OB_SUCCESS == ret && i < batch_size; ++i)
    {
      if (NULL == (entity = entity_factory_->alloc())) {
        ret = OB_ALLOCATE_MEMORY_FAILED;
      } else {
        table_operation_result.set_entity(*entity);
        if (OB_FAIL(serialization::decode(buf, data_len, pos, table_operation_result))) {
          LOG_WARN("fail to decode array item", K(ret), K(i), K(batch_size), K(data_len),
                   K(pos), K(table_operation_result));
        } else if (OB_FAIL(push_back(table_operation_result))) {
          LOG_WARN("fail to add item to array", K(ret), K(i), K(batch_size));
        }
      }
    } // end for
  } else {
    // deep copy properties
    for (int64_t i = 0; OB_SUCCESS == ret && i < batch_size; ++i)
    {
      if (NULL == (entity = entity_factory_->alloc())) {
        ret = OB_ALLOCATE_MEMORY_FAILED;
      } else {
        entity->set_allocator(alloc_);
        table_operation_result.set_entity(*entity);
        if (OB_FAIL(serialization::decode(buf, data_len, pos, table_operation_result))) {
          LOG_WARN("fail to decode array item", K(ret), K(i), K(batch_size), K(data_len),
                   K(pos), K(table_operation_result));
        } else if (OB_FAIL(push_back(table_operation_result))) {
          LOG_WARN("fail to add item to array", K(ret), K(i), K(batch_size));
        }
      }
    } // end for
  }
  return ret;
}

OB_UNIS_DEF_SERIALIZE((ObTableBatchOperationResult, ObTableBatchOperationResult::BaseType), );
OB_UNIS_DEF_SERIALIZE_SIZE((ObTableBatchOperationResult, ObTableBatchOperationResult::BaseType), );
////////////////////////////////////////////////////////////////
void ObTableQuery::reset()
{
  deserialize_allocator_ = NULL;
  key_ranges_.reset();
  select_columns_.reset();
  filter_string_.reset();
  limit_ = -1;  // no limit
  offset_ = 0;
  scan_order_ = ObQueryFlag::Forward;
  index_name_.reset();
  batch_size_ = -1;
  max_result_size_ = -1;
}

bool ObTableQuery::is_valid() const
{
  return (limit_ == -1 || limit_ > 0)
      && (offset_ >= 0)
      && key_ranges_.count() > 0
      && select_columns_.count() > 0;
}

int ObTableQuery::add_scan_range(common::ObNewRange &scan_range)
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(key_ranges_.push_back(scan_range))) {
    LOG_WARN("failed to add rowkey range", K(ret), K(scan_range));
  }
  return ret;
}

int ObTableQuery::set_scan_order(common::ObQueryFlag::ScanOrder scan_order)
{
  int ret = OB_SUCCESS;
  if (scan_order != ObQueryFlag::Forward
      && scan_order != ObQueryFlag::Reverse) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid scan order", K(ret), K(scan_order));
  } else {
    scan_order_ = scan_order;
  }
  return ret;
}

int ObTableQuery::add_select_column(const ObString &column)
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(select_columns_.push_back(column))) {
    LOG_WARN("failed to add select column", K(ret), K(column));
  }
  return ret;
}

int ObTableQuery::set_scan_index(const ObString &index_name)
{
  index_name_ = index_name;
  return OB_SUCCESS;
}

int ObTableQuery::set_filter(const ObString &filter)
{
  LOG_WARN("general filter not supported", K(filter));
  return OB_NOT_SUPPORTED;
}

int ObTableQuery::set_limit(int32_t limit)
{
  int ret = OB_SUCCESS;
  if (limit < -1 || 0 == limit) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("limit cannot be negative or zero", K(ret), K(limit));
  } else {
    limit_ = limit;
  }
  return ret;
}

int ObTableQuery::set_offset(int32_t offset)
{
  int ret = OB_SUCCESS;
  if (offset < 0) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("offset cannot be negative", K(ret), K(offset));
  } else {
    offset_ = offset;
  }
  return ret;
}

int ObTableQuery::set_batch(int32_t batch_size)
{
  int ret = OB_SUCCESS;
  if (batch_size == 0) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("batch_size cannot be zero", K(ret), K(batch_size));
  } else {
    batch_size_ = batch_size;
  }
  return ret;
}

int ObTableQuery::set_max_result_size(int64_t max_result_size)
{
  int ret = OB_SUCCESS;
  if (max_result_size == 0) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("max_result_size cannot be zero", K(ret), K(max_result_size));
  } else {
    max_result_size_ = max_result_size;
  }
  return ret;
}

uint64_t ObTableQuery::get_checksum() const
{
  uint64_t checksum = 0;
  const int64_t range_count = get_range_count();
  checksum = ob_crc64(checksum, &range_count, sizeof(range_count));
  for (int64_t i = 0; i < select_columns_.count(); ++i) {
    const ObString &cur_column = select_columns_.at(i);
    checksum = ob_crc64(checksum, cur_column.ptr(), cur_column.length());
  }
  checksum = ob_crc64(checksum, filter_string_.ptr(), filter_string_.length());
  checksum = ob_crc64(checksum, &limit_, sizeof(limit_));
  checksum = ob_crc64(checksum, &offset_, sizeof(offset_));
  checksum = ob_crc64(checksum, &scan_order_, sizeof(scan_order_));
  checksum = ob_crc64(checksum, index_name_.ptr(), index_name_.length());
  checksum = ob_crc64(checksum, &batch_size_, sizeof(batch_size_));
  checksum = ob_crc64(checksum, &max_result_size_, sizeof(max_result_size_));
  return checksum;
}

OB_UNIS_DEF_SERIALIZE(ObTableQuery,
                      key_ranges_,
                      select_columns_,
                      filter_string_,
                      limit_,
                      offset_,
                      scan_order_,
                      index_name_,
                      batch_size_,
                      max_result_size_);

OB_UNIS_DEF_SERIALIZE_SIZE(ObTableQuery,
                           key_ranges_,
                           select_columns_,
                           filter_string_,
                           limit_,
                           offset_,
                           scan_order_,
                           index_name_,
                           batch_size_,
                           max_result_size_);

OB_DEF_DESERIALIZE(ObTableQuery,)
{
  int ret = OB_SUCCESS;
  UNF_UNUSED_DES;
  if (OB_SUCC(ret)) {
    int64_t count = 0;
    key_ranges_.reset();
    if (OB_FAIL(serialization::decode_vi64(buf, data_len, pos, &count))) {
      LOG_WARN("fail to decode key ranges count", K(ret));
    }
    for (int64_t i = 0; OB_SUCC(ret) && i < count; i ++) {
      if (OB_ISNULL(deserialize_allocator_)) {
        ret = OB_NOT_INIT;
        LOG_WARN("deserialize allocator is NULL", K(ret));
      } else {
        ObObj array[OB_MAX_ROWKEY_COLUMN_NUMBER * 2];
        ObNewRange copy_range;
        ObNewRange key_range;
        copy_range.start_key_.assign(array, OB_MAX_ROWKEY_COLUMN_NUMBER);
        copy_range.end_key_.assign(array + OB_MAX_ROWKEY_COLUMN_NUMBER, OB_MAX_ROWKEY_COLUMN_NUMBER);
        if (OB_FAIL(copy_range.deserialize(buf, data_len, pos))) {
          LOG_WARN("fail to deserialize range", K(ret));
        } else if (OB_FAIL(common::deep_copy_range(*deserialize_allocator_, copy_range, key_range))) {
          LOG_WARN("fail to deep copy range", K(ret));
        } else if (OB_FAIL(key_ranges_.push_back(key_range))) {
          LOG_WARN("fail to add key range to array", K(ret));
        }
      }
    }
  }
  if (OB_SUCC(ret)) {
    LST_DO_CODE(OB_UNIS_DECODE,
                select_columns_,
                filter_string_,
                limit_,
                offset_,
                scan_order_,
                index_name_,
                batch_size_,
                max_result_size_
                );
  }
  return ret;
}

////////////////////////////////////////////////////////////////
ObTableEntityIterator::~ObTableEntityIterator()
{}

////////////////////////////////////////////////////////////////
ObTableQueryResult::ObTableQueryResult()
    :row_count_(0),
     allocator_(ObModIds::TABLE_PROC),
     fixed_result_size_(0),
     curr_idx_(0)
{
}

void ObTableQueryResult::reset_except_property()
{
  row_count_ = 0;
  buf_.reset();
  allocator_.reset();
  fixed_result_size_ = 0;
  curr_idx_ = 0;
  curr_entity_.reset();
}

void ObTableQueryResult::reset()
{
  properties_names_.reset();
  reset_except_property();
}

void ObTableQueryResult::rewind()
{
  curr_idx_ = 0;
  buf_.get_position() = 0;
}

int ObTableQueryResult::get_next_entity(const ObITableEntity *&entity)
{
  int ret = OB_SUCCESS;
  if (0 >= properties_names_.count()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid properties_names", K(ret));
  } else if (curr_idx_ >= row_count_) {
    ret = OB_ITER_END;
  } else {
    curr_entity_.reset();
    ObObj value;
    const int64_t N = properties_names_.count();
    for (int i = 0; OB_SUCCESS == ret && i < N; ++i)
    {
      if (OB_FAIL(value.deserialize(buf_.get_data(), buf_.get_capacity(), buf_.get_position()))) {
        LOG_WARN("failed to deserialize obj", K(ret), K_(buf));
      } else if (OB_FAIL(curr_entity_.set_property(properties_names_.at(i), value))) {
        LOG_WARN("failed to set entity property", K(ret), K(i), K(value));
      }
    } // end for
    if (OB_SUCC(ret)) {
      entity = &curr_entity_;
      ++curr_idx_;
    }
  }
  return ret;
}

int ObTableQueryResult::get_first_row(common::ObNewRow &row) const
{
  int ret = OB_SUCCESS;
  if (row.is_invalid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid row object", K(ret));
  } else if (0 >= row_count_) {
    ret = OB_ITER_END;
  } else {
    const char *databuf = buf_.get_data();
    const int64_t datalen = buf_.get_position();
    int64_t pos = 0;
    const int64_t N = row.count_;
    for (int i = 0; OB_SUCCESS == ret && i < N; ++i)
    {
      if (OB_FAIL(row.cells_[i].deserialize(databuf, datalen, pos))) {
        LOG_WARN("failed to deserialize obj", K(ret), K(datalen), K(pos));
      }
    } // end for
  }
  return ret;
}

int ObTableQueryResult::add_property_name(const ObString &name)
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(properties_names_.push_back(name))) {
    LOG_WARN("failed to add name", K(ret), K(name));
  }
  return ret;
}

int ObTableQueryResult::alloc_buf_if_need(const int64_t need_size)
{
  int ret = OB_SUCCESS;
  if (need_size <= 0 || need_size > MAX_BUF_BLOCK_SIZE) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument", K(need_size), LITERAL_K(MAX_BUF_BLOCK_SIZE));
  } else if (NULL == buf_.get_data()) { // first alloc
    int64_t actual_size = 0;
    if (need_size <= DEFAULT_BUF_BLOCK_SIZE) {
      actual_size = DEFAULT_BUF_BLOCK_SIZE;
    } else {
      actual_size = need_size;
    }
    char *tmp_buf = static_cast<char*>(allocator_.alloc(actual_size));
    if (NULL == tmp_buf) {
      ret = OB_ALLOCATE_MEMORY_FAILED;
      LOG_WARN("no memory", K(ret), K(actual_size));
    } else {
      buf_.set_data(tmp_buf, actual_size);
    }
  } else if (buf_.get_remain() < need_size) {
    if (need_size + buf_.get_position() > MAX_BUF_BLOCK_SIZE) { // check max buf size when expand buf
      ret = OB_BUF_NOT_ENOUGH;
      LOG_WARN("will exceed max buf need_size", K(ret), K(need_size), K(buf_.get_position()), LITERAL_K(MAX_BUF_BLOCK_SIZE));
    } else {
      int64_t actual_size = MAX(need_size + buf_.get_position(), 2 * buf_.get_capacity());
      actual_size = MIN(actual_size, MAX_BUF_BLOCK_SIZE);
      char *tmp_buf = static_cast<char*>(allocator_.alloc(actual_size));
      if (NULL == tmp_buf) {
        ret = OB_ALLOCATE_MEMORY_FAILED;
        LOG_WARN("no memory", K(ret), K(actual_size));
      } else {
        const int64_t old_buf_size = buf_.get_position();
        MEMCPY(tmp_buf, buf_.get_data(), old_buf_size);
        buf_.set_data(tmp_buf, actual_size);
        buf_.get_position() = old_buf_size;
      }
    }

  }
  return ret;
}

int ObTableQueryResult::add_row(const ObNewRow &row)
{
  int ret = OB_SUCCESS;
  ret = alloc_buf_if_need(row.get_serialize_size());
  const int64_t N = row.get_count();
  if (OB_SUCC(ret)) {
    if (0 != properties_names_.count()
        && N != properties_names_.count()) {
      ret = OB_INVALID_ARGUMENT;
      LOG_WARN("cell count not match with property count", K(ret), K(N),
               "properties_count", properties_names_.count());
    }
  }
  for (int i = 0; OB_SUCCESS == ret && i < N; ++i)
  {
    if (OB_FAIL(row.get_cell(i).serialize(buf_.get_data(), buf_.get_capacity(), buf_.get_position()))) {
      LOG_WARN("failed to serialize obj", K(ret), K_(buf));
    }
  } // end for
  if (OB_SUCC(ret)) {
    ++row_count_;
  }
  return ret;
}

int ObTableQueryResult::add_all_property(const ObTableQueryResult &other)
{
  int ret = OB_SUCCESS;
  if (0 != properties_names_.count()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid properties which has been initialized", K(ret));
  } else if (OB_FAIL(append(properties_names_, other.properties_names_))) {
    LOG_WARN("failed to append property", K(ret));
  }
  return ret;
}

int ObTableQueryResult::add_all_row(const ObTableQueryResult &other)
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(alloc_buf_if_need(other.buf_.get_position()))) {
  } else if (buf_.get_remain() < other.buf_.get_position()) {
    ret = OB_BUF_NOT_ENOUGH;
  } else {
    MEMCPY(buf_.get_cur_pos(), other.buf_.get_data(), other.buf_.get_position());
    buf_.get_position() += other.buf_.get_position();
    row_count_ += other.row_count_;
  }
  return ret;
}

int64_t ObTableQueryResult::get_result_size()
{
  if (0 >= fixed_result_size_) {
    fixed_result_size_ = properties_names_.get_serialize_size();
    fixed_result_size_ += obrpc::ObRpcPacketHeader::HEADER_SIZE
                          + 8/*appr. row_count*/
                          + 8/*appr. buf_position*/;
  }
  return fixed_result_size_ + buf_.get_position();
}

bool ObTableQueryResult::reach_batch_size_or_result_size(const int32_t batch_count,
                                                         const int64_t max_result_size)
{
  bool reach_size = false;
  if (batch_count > 0 && this->get_row_count() >= batch_count) {
    LOG_DEBUG("[yzfdebug] reach batch limit", K(batch_count));
    reach_size = true;
  } else if (max_result_size > 0 && this->get_result_size() >= max_result_size) {
    LOG_DEBUG("[yzfdebug] reach size limit", K(max_result_size));
    reach_size = true;
  }
  return reach_size;
}

OB_DEF_SERIALIZE(ObTableQueryResult)
{
  int ret = OB_SUCCESS;
  LST_DO_CODE(OB_UNIS_ENCODE,
              properties_names_,
              row_count_,
              buf_.get_position());
  if (OB_SUCC(ret)) {
    if (buf_len - pos < buf_.get_position()) {
      LOG_WARN("failed to serialize ObTableQueryResult", K(ret), K(buf_len), K(pos), "datalen", buf_.get_position());
    } else {
      MEMCPY(buf+pos, buf_.get_data(), buf_.get_position());
      pos += buf_.get_position();
    }
  }
  return ret;
}

OB_DEF_SERIALIZE_SIZE(ObTableQueryResult)
{
  int64_t len = 0;
  LST_DO_CODE(OB_UNIS_ADD_LEN,
              properties_names_,
              row_count_,
              buf_.get_position());
  len += buf_.get_position();
  return len;
}

OB_DEF_DESERIALIZE(ObTableQueryResult)
{
  int ret = OB_SUCCESS;
  allocator_.reset();  // deep copy all
  properties_names_.reset();
  curr_idx_ = 0;
  int64_t databuff_len = 0;
  int64_t properties_count = 0;
  OB_UNIS_DECODE(properties_count);
  if (OB_SUCC(ret)) {
    ObString property_name;
    ObString name_clone;
    for (int64_t i = 0; OB_SUCCESS == ret && i < properties_count; ++i)
    {
      OB_UNIS_DECODE(property_name);
      if (OB_SUCC(ret)) {
        if (OB_FAIL(ob_write_string(allocator_, property_name, name_clone))) {
          LOG_WARN("failed to deep copy string", K(ret), K(property_name));
        } else if (OB_FAIL(properties_names_.push_back(name_clone))) {
          LOG_WARN("failed to push back", K(ret));
        }
      }
    } // end for
  }
  LST_DO_CODE(OB_UNIS_DECODE,
              row_count_,
              databuff_len);
  char *buff1 = NULL;
  if (OB_FAIL(ret)) {
  } else if (databuff_len > data_len - pos) {
    ret = OB_ERR_UNEXPECTED;
    LOG_ERROR("invalid data", K(ret), K(databuff_len), K(pos), K(data_len));
  } else if (databuff_len == 0) {
    buf_.set_data(NULL, 0);
  } else if (NULL == (buff1 = static_cast<char*>(allocator_.alloc(databuff_len)))) {
    ret = OB_ALLOCATE_MEMORY_FAILED;
    LOG_WARN("no memory", K(ret), K(databuff_len));
  } else {
    // deep copy
    MEMCPY(buff1, buf+pos, databuff_len);
    buf_.set_data(buff1, databuff_len);
    pos += databuff_len;
  }
  return ret;
}
