#ifndef MATADOR_SELECT_QUERY_BUILDER_HPP
#define MATADOR_SELECT_QUERY_BUILDER_HPP

#include "matador/query/access.hpp"
#include "matador/query/basic_schema.hpp"
#include "matador/query/column_options.hpp"
#include "matador/query/criteria.hpp"
#include "matador/query/error_code.hpp"
#include "matador/query/foreign_key_options.hpp"
#include "matador/query/internal/criteria_transformer.hpp"
#include "matador/query/join_columns_collector.hpp"
#include "matador/query/object_ptr.hpp"
#include "matador/query/primary_key_options.hpp"
#include "matador/query/query.hpp"
#include "matador/query/statement.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/error_exception.hpp"
#include "matador/utils/result.hpp"

#include <deque>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace matador::query {

struct select_query_data {
  const table* root_table{nullptr};
  std::string pk_column_name{};
  std::vector<column> columns{};
  std::vector<join_data> joins{};
  criteria_ptr where_clause{};
};

/**
 * Builds a select query for an entity including all eagerly
 * fetched relations. The generated query references tables owned
 * by the builder, therefore the builder must outlive the compilation
 * of the returned query. Every call to build() starts with a fresh state.
 */
class select_query_builder final {
public:
  explicit select_query_builder(const basic_schema &scm)
  : schema_(scm) {}

  template<class EntityType>
  utils::result<fetchable_query, utils::error> build(criteria_ptr clause = {}) {
    try {
      reset();
      const auto node = resolve(typeid(EntityType));
      const table &root = visit_entity<EntityType>(node);
      state_.data.root_table = &root;

      if (clause) {
        criteria_transformer transformer{state_.processed};
        clause->accept(transformer);
        state_.data.where_clause = std::move(clause);
      }

      fetchable_query q = select(state_.data.columns)
        .from(root)
        .join_left(state_.data.joins)
        .where(std::move(state_.data.where_clause))
        .order_by(column::make_plain(&root, state_.data.pk_column_name))
        .asc();

      return {utils::ok(std::move(q))};
    } catch (const error_exception &ex) {
      return {utils::failure(ex.error())};
    } catch (const std::exception &ex) {
      return utils::failure{utils::error(error_code::UnexpectedError, ex.what())};
    }
  }

  template<typename BaseType>
  static void on_base(const BaseType &) {}

  template<class V>
  void on_primary_key(const char *id, V &, const primary_key_options & /*attr*/) {
    push(id);
    if (is_root_entity()) {
      state_.data.pk_column_name = id;
    }
  }

  void on_revision(const char *id, uint64_t & /*rev*/);

  template<typename Type>
  void on_attribute(const char *id, Type &, const column_options & /*attr*/) {
    push(id);
  }

  template<class Pointer>
  void on_belongs_to(const char *id, Pointer & /*obj*/, const foreign_key_options &attr) {
    using foreign_type = typename Pointer::value_type;
    const auto node = resolve(typeid(foreign_type), true);
    if (attr.fetch() != fetch_type::Eager) {
      push(id);
      return;
    }
    if (is_processed(node)) {
      return;
    }

    const table &parent = current_table();
    const table &foreign = visit_entity<foreign_type>(node);
    append_join(
      column::make_plain(&parent, id),
      column::make_plain(&foreign, primary_key_name(node->info()))
    );
  }

  template<class Pointer>
  void on_has_one(const char * /*id*/, Pointer & /*obj*/, const char *join_column, const foreign_key_options &attr) {
    on_child_entity<typename Pointer::value_type>(join_column, attr);
  }

  template<class CollectionType>
  void on_has_many(const char * /*id*/, CollectionType &, const char *join_column, const foreign_key_options &attr) {
    if constexpr (is_object_ptr<typename CollectionType::value_type>::value) {
      on_child_entity<typename CollectionType::value_type::value_type>(join_column, attr);
    }
  }

  template<class ContainerType>
  void on_has_many_to_many(const char *id, ContainerType &, const char *join_column, const char *inverse_join_column, const foreign_key_options &attr) {
    on_relation_entity<typename ContainerType::value_type::value_type>(id, join_column, inverse_join_column, attr);
  }

  template<class ContainerType>
  void on_has_many_to_many(const char *id, ContainerType &, const foreign_key_options &attr) {
    using related_type = typename ContainerType::value_type::value_type;
    if (attr.fetch() != fetch_type::Eager) {
      return;
    }
    const auto columns = join_columns_collector{}.collect<related_type>();
    on_relation_entity<related_type>(id, columns.inverse_join_column, columns.join_column, attr);
  }

  [[nodiscard]] const select_query_data &query_data() const;

private:
  struct scope {
    const basic_table_info *info;
    const table *tab;
  };

  struct build_state {
    std::deque<table> tables{};
    std::unordered_map<std::string, const table *> processed{};
    std::vector<scope> scopes{};
    select_query_data data{};
    unsigned int column_index{0};
    unsigned int table_index{0};
  };

  void reset();
  void push(const std::string &column_name);
  static std::string build_alias(char prefix, unsigned int count);
  [[nodiscard]] bool is_root_entity() const;
  [[nodiscard]] const table &current_table() const;
  [[nodiscard]] const basic_table_info &current_info() const;
  void append_join(const column &left, const column &right);

  // Looks up the schema node and optionally requires a primary key; throws error_exception
  [[nodiscard]] basic_schema::const_iterator resolve(const std::type_index &type, bool require_primary_key = true) const;
  [[nodiscard]] static const std::string &primary_key_name(const basic_table_info &info);
  [[nodiscard]] bool is_processed(const basic_schema::const_iterator &node) const;
  const table &add_table(const basic_table_info &info, const std::string &key);

  // Adds the table of the entity and its columns and relations to the query
  template<class EntityType>
  const table &visit_entity(const basic_schema::const_iterator &node) {
    const table &tab = add_table(node->info(), node->name());
    state_.scopes.push_back({&node->info(), &tab});
    EntityType obj;
    access::process(*this, obj);
    state_.scopes.pop_back();
    return tab;
  }

  // Joins an entity holding the foreign key (join_column) to the current entity
  template<class ChildType>
  void on_child_entity(const char *join_column, const foreign_key_options &attr) {
    if (attr.fetch() != fetch_type::Eager) {
      return;
    }
    const auto node = resolve(typeid(ChildType));
    if (is_processed(node)) {
      return;
    }

    const table &parent = current_table();
    const std::string &parent_pk = primary_key_name(current_info());
    const table &child = visit_entity<ChildType>(node);
    append_join(
      column::make_plain(&parent, parent_pk),
      column::make_plain(&child, join_column)
    );
  }

  // Joins an entity over its relation table, to_parent_column/to_related_column are columns of the relation table
  template<class RelatedType>
  void on_relation_entity(const char *relation_name, const std::string &to_parent_column, const std::string &to_related_column, const foreign_key_options &attr) {
    if (attr.fetch() != fetch_type::Eager) {
      return;
    }
    const auto node = resolve(typeid(RelatedType));
    if (is_processed(node)) {
      return;
    }

    const table *relation = nullptr;
    if (const auto it = state_.processed.find(relation_name); it != state_.processed.end()) {
      relation = it->second;
    } else {
      relation = &add_table(resolve(relation_name, false)->info(), relation_name);
    }

    const table &parent = current_table();
    const std::string &parent_pk = primary_key_name(current_info());
    const table &related = visit_entity<RelatedType>(node);
    append_join(
      column::make_plain(&parent, parent_pk),
      column::make_plain(relation, to_parent_column)
    );
    append_join(
      column::make_plain(relation, to_related_column),
      column::make_plain(&related, primary_key_name(node->info()))
    );
  }

  [[nodiscard]] basic_schema::const_iterator resolve(const std::string &name, bool require_primary_key) const;

private:
  const basic_schema &schema_;
  build_state state_{};
};
}
#endif //MATADOR_SELECT_QUERY_BUILDER_HPP
