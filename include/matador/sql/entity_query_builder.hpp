#ifndef QUERY_ENTITY_QUERY_BUILDER_HPP
#define QUERY_ENTITY_QUERY_BUILDER_HPP

#include "matador/sql/connection.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/query_context.hpp"
#include "matador/sql/query.hpp"
#include "matador/sql/query_intermediates.hpp"
#include "matador/sql/value.hpp"

#include "matador/utils/result.hpp"

#include <iostream>

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
    join_columns_ = {};
    Type obj;

    matador::access::process(*this, obj);

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

struct entity_query_data {
  std::string root_table_name;
  std::string pk_column_{};
  std::vector<column> columns {};
  std::vector<join_data> joins{};
  std::unique_ptr<basic_condition> where_clause{};
};

enum class query_build_error : std::uint8_t {
  Ok = 0,
  UnknownType,
  MissingPrimaryKey,
  UnexpectedError
};

class query_builder_exception : public std::exception
{
public:
  explicit query_builder_exception(query_build_error error) : error_(error) {}

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
    table_info_stack_.push(info.value());
    char str[4];
    snprintf(str, 4, "T%02d", static_cast<int>(table_info_stack_.size()));
    table_stack_.emplace(info.value().name, str);
    entity_query_data_ = { info->name };
    try {
    EntityType obj;
      access::process(*this, obj);

      return {utils::ok(std::move(entity_query_data_))};
    } catch (const query_builder_exception &ex) {
      return {utils::error(ex.error())};
    } catch (...) {
      return {utils::error(query_build_error::UnexpectedError)};
    }
  }

  template<class EntityType>
  utils::result<entity_query_data, query_build_error> build() {
    const auto info = schema_.info<EntityType>();
    if (!info) {
      return utils::error(query_build_error::UnknownType);
    }
    pk_ = nullptr;
    table_info_stack_.push(info.value());
    entity_query_data_ = { info->name };
    try {
        EntityType obj;
        access::process(*this, obj);

      return {utils::ok(std::move(entity_query_data_))};
    } catch (const query_builder_exception &ex) {
      return {utils::error(ex.error())};
    } catch (...) {
      return {utils::error(query_build_error::UnexpectedError)};
    }
  }

  template < class V >
  void on_primary_key(const char *id, V &, std::enable_if_t<std::is_integral_v<V> && !std::is_same_v<bool, V>>* = nullptr)
  {
    push(id);
      if (!is_root_entity()) {
          return;
      }
      if (pk_.is_null()) {
          entity_query_data_.pk_column_ = id;
      } else if (pk_.is_integer()) {
          entity_query_data_.where_clause = make_condition(column{table_stack_.top().alias, id, ""} == *pk_.as<V>());
          entity_query_data_.pk_column_ = id;
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
      typename ContainerType::value_type::value_type obj;
      matador::access::process(*this , obj);
      table_info_stack_.pop();

      auto pk = info->prototype.primary_key();
      if (!pk) {
        throw query_builder_exception{query_build_error::MissingPrimaryKey};
      }

      append_join({table_stack_.top().name, table_info_stack_.top().prototype.primary_key()->name()}, {info->name, join_column}, table_stack_.top());
    }
  }

  template<class ContainerType>
  void on_has_many_to_many(const char *id, ContainerType &/*c*/, const char *join_column, const char *inverse_join_column, const utils::foreign_attributes &attr)
  {
    if (attr.fetch() != utils::fetch_type::EAGER) {
      return;
    }
    const auto info = schema_.info<typename ContainerType::value_type::value_type>();
    if (!info) {
      throw query_builder_exception{query_build_error::UnknownType};
    }
    table_info_stack_.push(info.value());
    typename ContainerType::value_type::value_type obj;
    matador::access::process(*this , obj);
    table_info_stack_.pop();

    auto pk = info->prototype.primary_key();
    if (!pk) {
      throw query_builder_exception{query_build_error::MissingPrimaryKey};
    }

    append_join({table_stack_.top().name, table_info_stack_.top().prototype.primary_key()->name()}, {id, join_column}, table_stack_.top());
    append_join({id, inverse_join_column}, {info->name, pk->name()}, {info->name, "TTT"});
  }

  template<class ContainerType>
  void on_has_many_to_many(const char *id, ContainerType &/*c*/, const utils::foreign_attributes &attr)
  {
    if (attr.fetch() != utils::fetch_type::EAGER) {
      return;
    }
    const auto info = schema_.info<typename ContainerType::value_type::value_type>();
    if (!info) {
      throw query_builder_exception{query_build_error::UnknownType};
    }
    table_info_stack_.push(info.value());
    typename ContainerType::value_type::value_type obj;
    matador::access::process(*this , obj);
    table_info_stack_.pop();

    auto pk = info->prototype.primary_key();
    if (!pk) {
      throw query_builder_exception{query_build_error::MissingPrimaryKey};
    }

    const auto join_columns = join_column_collector_.collect<typename ContainerType::value_type::value_type>();

    append_join({table_info_stack_.top().name, table_info_stack_.top().prototype.primary_key()->name()}, {id, join_columns.inverse_join_column}, table_stack_.top());
    append_join({id, join_columns.join_column}, {info->name, pk->name()}, {info->name, "TTT"});
  }

private:
  template<class Pointer>
  void on_foreign_object(const char *id, Pointer &, const utils::foreign_attributes &attr);
  void push(const std::string &column_name);
  [[nodiscard]] bool is_root_entity() const;
  void append_join(const column &left, const column &right, const sql::table& join_table);

private:
  value pk_;
  std::stack<table_info> table_info_stack_;
  std::stack<table> table_stack_;
  const schema &schema_;
  entity_query_data entity_query_data_;
  int column_index{0};
  join_column_collector join_column_collector_;
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
    typename Pointer::value_type obj;
    matador::access::process(*this, obj);
    table_info_stack_.pop();

    auto pk = info->prototype.primary_key();
    if (!pk) {
      throw query_builder_exception{query_build_error::MissingPrimaryKey};
    }
    append_join({table_info_stack_.top().name, id}, {info->name, pk->name()}, table_stack_.top());
  } else {
    push(id);
  }
}

}
#endif //QUERY_ENTITY_QUERY_BUILDER_HPP
