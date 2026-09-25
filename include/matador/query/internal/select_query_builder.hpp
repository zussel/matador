#ifndef QUERY_ENTITY_QUERY_BUILDER_HPP
#define QUERY_ENTITY_QUERY_BUILDER_HPP

#include "matador/query/criteria.hpp"
#include "matador/query/error_code.hpp"
#include "matador/query/query.hpp"
#include "matador/query/criteria/criteria_visitor.hpp"
#include "matador/query/basic_schema.hpp"
#include "matador/query/statement.hpp"
#include "matador/query/join_columns_collector.hpp"
#include "matador/query/primary_key_options.hpp"
#include "matador/query/column_value.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"
#include "matador/utils/error_exception.hpp"

#include <stack>
#include <unordered_map>

namespace matador::query {

struct select_query_data {
  const table* root_table{nullptr};
  std::string pk_column_name{};
  std::vector<column> columns{};
  std::vector<join_data> joins{};
  criteria_ptr where_clause{};
};

class criteria_transformer final : public criteria_visitor {
public:
    criteria_transformer(const basic_schema &repo, const std::unordered_map<std::string, table>& tables_by_name);
    void visit( const between_criteria& node ) override;
    void visit( const binary_criteria& node ) override;
    void visit( const binary_column_criteria& node ) override;
    void visit( const collection_criteria& node ) override;
    void visit( const check_null_criteria& node ) override;
    void visit( const collection_query_criteria& node ) override;
    void visit( const like_criteria& node ) override;
    void visit( const logical_criteria& node ) override;
    void visit( const not_criteria& node ) override;

private:
    void update_criteria_column(const abstract_column_criteria& node) const;

private:
    const basic_schema &repo_;
    const std::unordered_map<std::string, table>& tables_by_name_;
};

class select_query_builder final {
public:
  explicit select_query_builder(const basic_schema &scm)
  : schema_(scm){}

  template<class EntityType>
  utils::result<fetchable_query, utils::error> build(criteria_ptr clause = {}) {
    const auto it = schema_.find(typeid(EntityType));
    if (it == schema_.end()) {
      return utils::failure{utils::error(error_code::UnknownType, "Entity type not found in schema")};
    }
    table_info_stack_.push({it->info(), it->second.table().as(build_alias('t', ++table_index))});
    entity_query_data_ = { &table_info_stack_.top().table };
    processed_tables_.insert({it->second.name(), *entity_query_data_.root_table});
    try {
      EntityType obj;
      access::process(*this, obj);

      if (clause) {
        criteria_transformer transformer{schema_, processed_tables_};
        clause->accept(transformer);
        entity_query_data_.where_clause = std::move(clause);
      }

      fetchable_query q = select(entity_query_data_.columns)
        .from(*entity_query_data_.root_table)
        .join_left(entity_query_data_.joins)
        .where(std::move(entity_query_data_.where_clause))
        .order_by({entity_query_data_.root_table, entity_query_data_.pk_column_name})
        .asc();

      return {utils::ok(std::move(q))};
    } catch (const error_exception &ex) {
      return {utils::failure(ex.error())};
    } catch (...) {
      return utils::failure{utils::error(error_code::UnexpectedError, "Unexpected error")};
    }
  }

  template<typename BaseType>
  static void on_base(const BaseType&) {}

  template < class V >
  void on_primary_key(const char *id, V &, const primary_key_options& /*attr*/) {
    push(id);
    if (!is_root_entity()) {
      return;
    }
    entity_query_data_.pk_column_name = id;
  }

  void on_revision(const char *id, uint64_t &/*rev*/);

  template<typename Type>
  void on_attribute(const char *id, Type &, const column_options &/*attr*/)
  {
    push(id);
  }

  template<class Pointer>
  void on_belongs_to(const char *id, Pointer &/*obj*/, const foreign_key_options &attr) {
    const auto it = schema_.find(typeid(typename Pointer::value_type));
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }
    if (!it->info().has_primary_key()) {
      throw error_exception{error_code::MissingPrimaryKey, "Missing primary key"};
    }

    const auto& info = it->info();
    auto foreign_table = it->second.table().as(build_alias('t', ++table_index));
    if (attr.fetch() == fetch_type::Eager) {

      auto next = processed_tables_.find(info.name());
      if (next != processed_tables_.end()) {
        return;
      }
      table_info_stack_.push({info, std::move(foreign_table)});
      next = processed_tables_.insert({info.name(), table_info_stack_.top().table}).first;
      typename Pointer::value_type obj;
      access::process(*this, obj);
      table_info_stack_.pop();

      append_join(
        column{&table_info_stack_.top().table, id},
        column{&next->second, info.primary_key_attribute()->name()}
      );
    } else {
      push(id);
    }
  }

  template<class Pointer>
  void on_has_one(const char * /*id*/, Pointer &/*obj*/, const char * join_column, const foreign_key_options &attr) {
    const auto it = schema_.find(typeid(typename Pointer::value_type));
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }
    if (!it->info().has_primary_key()) {
      throw error_exception{error_code::MissingPrimaryKey, "Missing primary key"};
    }

    const auto& info = it->info();
    auto foreign_table = it->second.table().as(build_alias('t', ++table_index));
    if (attr.fetch() == fetch_type::Eager) {

      auto next = processed_tables_.find(info.name());
      if (next != processed_tables_.end()) {
        return;
      }
      table_info_stack_.push({info, std::move(foreign_table)});
      next = processed_tables_.insert({info.name(), table_info_stack_.top().table}).first;
      typename Pointer::value_type obj;
      access::process(*this, obj);
      table_info_stack_.pop();

      append_join(
        column{&table_info_stack_.top().table, it->info().primary_key_attribute()->name()},
        column{&next->second, join_column}
      );
    }
  }

  template<typename T>
  struct NoopDeleter {
    void operator()(const T*) const {}
  };

  template<class CollectionType>
  void on_has_many(const char * /*id*/, CollectionType &, const char *join_column, const foreign_key_options &attr, std::enable_if_t<is_object_ptr<typename CollectionType::value_type>::value> * = nullptr) {
    if (attr.fetch() != fetch_type::Eager) {
      return;
    }

    const auto it = schema_.find(typeid(typename CollectionType::value_type::value_type));
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }

    auto next = processed_tables_.find(it->second.name());
    if (next != processed_tables_.end()) {
      // node already processed
      return;
    }

    table_info_stack_.push({it->info(), it->second.table().as(build_alias('t', ++table_index))});
    next = processed_tables_.insert({it->second.name(), table_info_stack_.top().table}).first;
    typename CollectionType::value_type::value_type obj;
    access::process(*this , obj);
    table_info_stack_.pop();

    if (!it->info().has_primary_key()) {
      throw error_exception{error_code::MissingPrimaryKey, "Missing primary key"};
    }

    append_join(
      column{&table_info_stack_.top().table, table_info_stack_.top().info.primary_key_attribute()->name()},
      column{&next->second, join_column}
    );
  }

  template<class CollectionType>
  void on_has_many(const char * /*id*/, CollectionType &, const char * /*join_column*/, const foreign_key_options &/*attr*/, std::enable_if_t<!is_object_ptr<typename CollectionType::value_type>::value> * = nullptr) {
  }

  template<class ContainerType>
  void on_has_many_to_many(const char *id, ContainerType &/*cont*/, const char *join_column, const char *inverse_join_column, const foreign_key_options &attr) {
    if (attr.fetch() != fetch_type::Eager) {
      return;
    }
    const auto result = schema_.find(typeid(typename ContainerType::value_type::value_type));
    if (result == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }

    auto next = processed_tables_.find(result->second.name());
    if (next != processed_tables_.end()) {
      // attribute was already processed
      return;
    }

    auto relation = processed_tables_.find(id);
    if (relation == processed_tables_.end()) {
      const auto it = schema_.find(id);
      if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
      }
      relation = processed_tables_.emplace(id, it->second.table().as(build_alias('t', ++table_index))).first;
    }
    table_info_stack_.push({result->info(), result->second.table().as(build_alias('t', ++table_index))});
    next = processed_tables_.insert({result->second.name(), table_info_stack_.top().table}).first;
    typename ContainerType::value_type::value_type obj;
    access::process(*this , obj);
    table_info_stack_.pop();

    if (!result->info().has_primary_key()) {
      throw error_exception{error_code::MissingPrimaryKey, "Missing primary key"};
    }

    append_join(
      column{&table_info_stack_.top().table, table_info_stack_.top().info.primary_key_attribute()->name()},
      column{&relation->second, join_column}
    );
    append_join(
      column{&relation->second, inverse_join_column},
      column{&next->second, result->info().primary_key_attribute()->name()}
    );
  }

  template<class ContainerType>
  void on_has_many_to_many(const char *id, ContainerType &/*cont*/, const foreign_key_options &attr) {
    if (attr.fetch() != fetch_type::Eager) {
      return;
    }
    const auto result = schema_.find(typeid(typename ContainerType::value_type::value_type));
    if (result == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }

    auto next = processed_tables_.find(result->second.name());
    if (next != processed_tables_.end()) {
      // attribute was already processed
      return;
    }

    auto relation = processed_tables_.find(id);
    if (relation == processed_tables_.end()) {
      const auto it = schema_.find(id);
      if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
      }
      const auto t = it->second.table().as(build_alias('t', ++table_index));
      relation = processed_tables_.insert({id, t}).first;
    }
    table_info_stack_.push({result->info(), result->second.table().as(build_alias('t', ++table_index))});
    next = processed_tables_.insert({result->second.name(), table_info_stack_.top().table}).first;
    typename ContainerType::value_type::value_type obj;
    access::process(*this , obj);
    table_info_stack_.pop();

    if (!result->info().has_primary_key()) {
      throw error_exception{error_code::MissingPrimaryKey, "Missing primary key"};
    }

    const auto join_columns = join_columns_collector_.collect<typename ContainerType::value_type::value_type>();

    append_join(
      column{&table_info_stack_.top().table, table_info_stack_.top().info.primary_key_attribute()->name()},
      column{&relation->second, join_columns.inverse_join_column}
    );
    append_join(
      column{&relation->second, join_columns.join_column},
      column{&next->second, result->info().primary_key_attribute()->name()}
    );
  }

  [[nodiscard]] const select_query_data &query_data() const;

private:
  void push(const std::string &column_name);
  static std::string build_alias(char prefix, unsigned int count);
  [[nodiscard]] bool is_root_entity() const;
  void append_join(const column &left, const column &right);

private:
  struct table_info {
    const basic_table_info &info;
    class table table;
  };

  std::stack<table_info> table_info_stack_{};
  std::unordered_map<std::string, table> processed_tables_{};
  const basic_schema &schema_;
  select_query_data entity_query_data_{};
  unsigned int column_index{0};
  unsigned int table_index{0};
  join_columns_collector join_columns_collector_{};
};
}
#endif //QUERY_ENTITY_QUERY_BUILDER_HPP
