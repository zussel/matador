#ifndef QUERY_ENTITY_QUERY_BUILDER_HPP
#define QUERY_ENTITY_QUERY_BUILDER_HPP

#include "matador/sql/connection.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/query_context.hpp"
#include "matador/sql/query_intermediates.hpp"
#include "matador/sql/value.hpp"

#include "matador/utils/result.hpp"

#include <iostream>
#include <unordered_set>

namespace matador::sql {

struct join_columns
{
  std::string join_column;
  std::string inverse_join_column;
};

class join_column_collector
{
public:
  template<class Type>
  join_columns collect()
  {
    Type obj;

    return collect(obj);
  }

  template<class Type>
  join_columns collect(const Type& obj)
  {
    join_columns_ = {};

    access::process(*this, obj);

    return join_columns_;
  }
  template < class V >
  void on_primary_key(const char * /*id*/, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char * /*id*/, std::string &, size_t) {}
  void on_revision(const char * /*id*/, unsigned long long &/*rev*/) {}
  template<typename Type>
  void on_attribute(const char * /*id*/, Type &, const utils::field_attributes &/*attr*/ = utils::null_attributes) {}
  template<class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer &obj, const utils::foreign_attributes &attr) {}
  template<class Pointer>
  void on_has_one(const char * /*id*/, Pointer &obj, const utils::foreign_attributes &attr) {}
  template<class ContainerType>
  void on_has_many(const char * /*id*/, ContainerType &, const char *join_column, const utils::foreign_attributes &attr) {}
  template<class ContainerType>
  void on_has_many_to_many(const char * /*id*/, ContainerType &/*c*/, const char *join_column, const char *inverse_join_column, const utils::foreign_attributes &/*attr*/)
  {
    join_columns_.join_column = join_column;
    join_columns_.inverse_join_column = inverse_join_column;
  }
  template<class ContainerType>
  void on_has_many_to_many(const char * /*id*/, ContainerType &/*c*/, const utils::foreign_attributes &/*attr*/) {}

private:
  join_columns join_columns_;
};

struct column_hash {
  std::size_t operator()(const column& x) const {
    return std::hash<std::string>()(x.name) ^ std::hash<std::string>()(x.alias);
  }
};

struct table_hash {
  std::size_t operator()(const table& x) const {
    return std::hash<std::string>()(x.name) ^ std::hash<std::string>()(x.alias);
  }
};

struct entity_query_data {
  entity_query_data() {
    tables.reserve(200);
    columns.reserve(500);
  }
  explicit entity_query_data(const table& root_table) {
    tables.reserve(200);
    columns.reserve(500);
    this->root_table = tables.emplace_back(std::make_shared<table>(root_table));
  }

  std::shared_ptr<table> root_table;
  std::vector<std::shared_ptr<table>> tables{};
  std::optional<std::reference_wrapper<const column>> pk_column_;
  std::vector<column> columns{};
  std::vector<join_data> joins{};
  std::unique_ptr<basic_condition> where_clause{};
};

enum class query_build_error : std::uint8_t {
  Ok = 0,
  UnknownType,
  MissingPrimaryKey,
  UnexpectedError
};

class query_builder_exception final : public std::exception
{
public:
  explicit query_builder_exception(const query_build_error error) : error_(error) {}

  [[nodiscard]] query_build_error error() const { return error_; }

private:
  const query_build_error error_;
};

class entity_query_builder
{
public:
  explicit entity_query_builder(const schema &scm)
  : schema_(scm) {}

  template<class EntityType, typename PrimaryKeyType>
  utils::result<entity_query_data, query_build_error> build(const PrimaryKeyType &pk) {
    const auto info = schema_.info<EntityType>();
    if (!info) {
      return utils::error(query_build_error::UnknownType);
    }
    pk_ = pk;
    setup_query_data(*info);
    try {
      EntityType obj;
      access::process(*this, obj);
      current_table_ = nullptr;

      return {utils::ok(std::move(entity_query_data_))};
    } catch (const query_builder_exception &ex) {
      return {utils::error(ex.error())};
    } catch (...) {
      return {utils::error(query_build_error::UnexpectedError)};
    }
  }

  template<class EntityType>
  utils::result<entity_query_data, query_build_error> build() {
    return build<EntityType>(nullptr);
  }

  template < class V >
  void on_primary_key(const char *id, V &, std::enable_if_t<std::is_integral_v<V> && !std::is_same_v<bool, V>>* = nullptr)
  {
    push(id);
    if (!is_root_entity()) {
      return;
    }
    if (pk_.is_integer()) {
      const auto it = column_ref_map_.find({current_table_, id});
      if (it == column_ref_map_.end()) {
        throw query_builder_exception{query_build_error::MissingPrimaryKey};
      }
      entity_query_data_.pk_column_.emplace(it->second);
      entity_query_data_.where_clause = make_condition(column{it->second.get().name, it->second.get().alias} == *pk_.as<V>());
    }
  }

  void on_primary_key(const char *id, std::string &, size_t);
  void on_revision(const char *id, unsigned long long &/*rev*/);

  template<typename Type>
  void on_attribute(const char *id, Type &, const utils::field_attributes &/*attr*/ = utils::null_attributes)
  {
    push(id);
  }

  template<class Pointer>
  void on_belongs_to(const char *id, Pointer &obj, const utils::foreign_attributes &attr)
  {
    on_foreign_object(id, obj, attr);
  }

  template<class Pointer>
  void on_has_one(const char *id, Pointer &obj, const utils::foreign_attributes &attr)
  {
    on_foreign_object(id, obj, attr);
  }

  template<class ContainerType>
  void on_has_many(const char * /*id*/, ContainerType &, const char *join_column, const utils::foreign_attributes &attr)
  {
    if (attr.fetch() == utils::fetch_type::EAGER) {
      const auto info = schema_.info<typename ContainerType::value_type::value_type>();
      if (!info) {
        throw query_builder_exception{query_build_error::UnknownType};
      }
      table_info_stack_.push(info.value());
      char str[4];
      snprintf(str, 4, "T%02d", static_cast<int>(table_info_stack_.size()));
      auto foreign_table = entity_query_data_.tables.emplace_back(std::make_shared<table>(info->name, str));
      current_table_ = foreign_table;
      typename ContainerType::value_type::value_type obj;
      access::process(*this , obj);
      current_table_ = entity_query_data_.root_table;
      table_info_stack_.pop();

      auto pk = info->prototype.primary_key();
      if (!pk) {
        throw query_builder_exception{query_build_error::MissingPrimaryKey};
      }

      append_join({entity_query_data_.root_table, table_info_stack_.top().prototype.primary_key()->name()}, {foreign_table, join_column});
    }
  }

  template<class ContainerType>
  void on_has_many_to_many(const char *id, ContainerType &/*c*/, const char *join_column, const char *inverse_join_column, const utils::foreign_attributes &attr)
  {
    if (attr.fetch() != utils::fetch_type::EAGER) {
      return;
    }
    const auto relation_info = schema_.info(id);
    if (!relation_info) {
      throw query_builder_exception{query_build_error::UnknownType};
    }
    table_info_stack_.push(relation_info.value());
    char str[4];
    snprintf(str, 4, "T%02d", static_cast<int>(table_info_stack_.size()));
    auto relation_table = entity_query_data_.tables.emplace_back(std::make_shared<table>(relation_info->name, str));
    const auto info = schema_.info<typename ContainerType::value_type::value_type>();
    if (!info) {
      throw query_builder_exception{query_build_error::UnknownType};
    }
    table_info_stack_.push(info.value());
    snprintf(str, 4, "T%02d", static_cast<int>(table_info_stack_.size()));
    auto foreign_table = entity_query_data_.tables.emplace_back(std::make_shared<table>(info->name, str));
    current_table_ = foreign_table;
    typename ContainerType::value_type::value_type obj;
    access::process(*this , obj);
    current_table_ = entity_query_data_.root_table;
    table_info_stack_.pop();
    table_info_stack_.pop();

    auto pk = info->prototype.primary_key();
    if (!pk) {
      throw query_builder_exception{query_build_error::MissingPrimaryKey};
    }

    append_join({entity_query_data_.root_table, table_info_stack_.top().prototype.primary_key()->name()}, {relation_table, join_column});
    append_join({relation_table, inverse_join_column}, {foreign_table, pk->name()});
  }

  template<class ContainerType>
  void on_has_many_to_many(const char *id, ContainerType &/*c*/, const utils::foreign_attributes &attr)
  {
    if (attr.fetch() != utils::fetch_type::EAGER) {
      return;
    }
      const auto relation_info = schema_.info(id);
      if (!relation_info) {
          throw query_builder_exception{query_build_error::UnknownType};
      }
      table_info_stack_.push(relation_info.value());
      char str[4];
      snprintf(str, 4, "T%02d", static_cast<int>(table_info_stack_.size()));
      auto relation_table = entity_query_data_.tables.emplace_back(std::make_shared<table>(relation_info->name, str));
      const auto info = schema_.info<typename ContainerType::value_type::value_type>();
    if (!info) {
      throw query_builder_exception{query_build_error::UnknownType};
    }
    table_info_stack_.push(info.value());
    snprintf(str, 4, "T%02d", static_cast<int>(table_info_stack_.size()));
    auto foreign_table = entity_query_data_.tables.emplace_back(std::make_shared<table>(info->name, str));
    current_table_ = foreign_table;
    typename ContainerType::value_type::value_type obj;
    access::process(*this , obj);
    current_table_ = entity_query_data_.root_table;
    table_info_stack_.pop();
    table_info_stack_.pop();

    auto pk = info->prototype.primary_key();
    if (!pk) {
      throw query_builder_exception{query_build_error::MissingPrimaryKey};
    }

    const auto join_columns = join_column_collector_.collect(obj);

    append_join({entity_query_data_.root_table, table_info_stack_.top().prototype.primary_key()->name()}, {relation_table, join_columns.inverse_join_column});
    append_join({relation_table, join_columns.join_column}, {foreign_table, pk->name()});
  }

private:
  struct column_key {
    std::shared_ptr<table> table;
    std::string name;

    bool operator==(const column_key &x) const {
      return name == x.name && *table == *x.table;
    }
  };

  struct column_key_hash {
    std::size_t operator()(const column_key& x) const {
      return std::hash<std::string>()(x.name) ^ std::hash<std::string>()(x.table->name);
    }
  };


private:
  void setup_query_data(const table_info& info);
  template<class Pointer>
  void on_foreign_object(const char *id, Pointer &, const utils::foreign_attributes &attr);
  void push(const std::string &column_name);
  [[nodiscard]] bool is_root_entity() const;
  void append_join(const column_key &left, const column_key &right);

private:
  value pk_;
  std::stack<table_info> table_info_stack_;
  std::shared_ptr<table> current_table_;
  const schema &schema_;
  entity_query_data entity_query_data_;
  int column_index{0};
  join_column_collector join_column_collector_;
  std::unordered_map<column_key, std::reference_wrapper<const column>, column_key_hash> column_ref_map_;
};

template<class Pointer>
void entity_query_builder::on_foreign_object(const char *id, Pointer &, const utils::foreign_attributes &attr)
{
  if (attr.fetch() == utils::fetch_type::EAGER) {
    const auto info = schema_.info<typename Pointer::value_type>();
    if (!info) {
      throw query_builder_exception{query_build_error::UnknownType};
    }
    table_info_stack_.push(info.value());
    char str[4];
    snprintf(str, 4, "T%02d", static_cast<int>(table_info_stack_.size()));
    auto foreign_table = entity_query_data_.tables.emplace_back(std::make_shared<table>(info->name, str));
    current_table_ = foreign_table;
    typename Pointer::value_type obj;
    access::process(*this, obj);
    current_table_ = entity_query_data_.root_table;
    table_info_stack_.pop();

    auto pk = info->prototype.primary_key();
    if (!pk) {
      throw query_builder_exception{query_build_error::MissingPrimaryKey};
    }
    append_join({entity_query_data_.root_table, id}, {foreign_table, pk->name()});
  } else {
    push(id);
  }
}

}
#endif //QUERY_ENTITY_QUERY_BUILDER_HPP
