#include "matador/query/internal/select_query_builder.hpp"
#include "matador/query/criteria.hpp"

#include <cstdio>

namespace matador::query {

void select_query_builder::on_revision(const char *id, uint64_t & /*rev*/) {
  push(id);
}

const select_query_data &select_query_builder::query_data() const {
  return state_.data;
}

void select_query_builder::reset() {
  state_ = {};
}

void select_query_builder::push(const std::string &column_name) {
  const auto &tab = current_table();
  const auto alias = build_alias('c', ++state_.column_index);
  if (const auto *col = tab.find_column(column_name); col != nullptr) {
    state_.data.columns.push_back(col->as(alias));
  } else {
    state_.data.columns.push_back(column::make_plain(&tab, column_name, alias));
  }
}

std::string select_query_builder::build_alias(const char prefix, const unsigned int count) {
  char buffer[16];
  std::snprintf(buffer, sizeof(buffer), "%c%02u", prefix, count);
  return buffer;
}

bool select_query_builder::is_root_entity() const {
  return state_.scopes.size() == 1;
}

const table &select_query_builder::current_table() const {
  return *state_.scopes.back().tab;
}

const basic_table_info &select_query_builder::current_info() const {
  return *state_.scopes.back().info;
}

void select_query_builder::append_join(const column &left, const column &right) {
  state_.data.joins.push_back({
    right.table(),
    std::make_unique<binary_column_criteria>(left, binary_operator::Equals, right)
  });
}

basic_schema::const_iterator select_query_builder::resolve(const std::type_index &type, const bool require_primary_key) const {
  const auto it = schema_.find(type);
  if (it == schema_.end()) {
    throw error_exception{error_code::UnknownType, "Unknown type"};
  }
  if (require_primary_key && !it->info().has_primary_key()) {
    throw error_exception{error_code::MissingPrimaryKey, "Type " + it->name() + " has no primary key"};
  }
  return it;
}

basic_schema::const_iterator select_query_builder::resolve(const std::string &name, const bool require_primary_key) const {
  const auto it = schema_.find(name);
  if (it == schema_.end()) {
    throw error_exception{error_code::UnknownType, "Unknown type " + name};
  }
  if (require_primary_key && !it->info().has_primary_key()) {
    throw error_exception{error_code::MissingPrimaryKey, "Type " + it->name() + " has no primary key"};
  }
  return it;
}

const std::string &select_query_builder::primary_key_name(const basic_table_info &info) {
  const auto *pk = info.primary_key_attribute();
  if (pk == nullptr) {
    throw error_exception{error_code::MissingPrimaryKey, "Type " + info.name() + " has no primary key"};
  }
  return pk->column_name();
}

bool select_query_builder::is_processed(const basic_schema::const_iterator &node) const {
  return state_.processed.find(node->name()) != state_.processed.end();
}

const table &select_query_builder::add_table(const basic_table_info &info, const std::string &key) {
  tables_.push_back(info.table()->as(build_alias('t', ++state_.table_index)));
  const table &tab = tables_.back();
  state_.processed.insert_or_assign(key, &tab);
  return tab;
}

}
