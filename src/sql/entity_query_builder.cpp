#include "matador/sql/entity_query_builder.hpp"

namespace matador::sql {

void entity_query_builder::on_primary_key(const char *id, std::string &, size_t)
{
  push(id);
  if (!is_root_entity()) {
    const auto b = pk_.is_varchar();
    std::cout << "is matching primary key: " << std::boolalpha << b << "\n";
  }
}

void entity_query_builder::on_revision(const char *id, unsigned long long &/*rev*/)
{
  push(id);
}

void entity_query_builder::setup_query_data(const table_info& info) {
  table_info_stack_.push(info);
  char str[4];
  snprintf(str, 4, "T%02d", static_cast<int>(table_info_stack_.size()));
  entity_query_data_ = {};
  entity_query_data_.tables.emplace_back(std::make_shared<table>(info.name, str));
  entity_query_data_.root_table = entity_query_data_.tables.back();
  current_table_ = entity_query_data_.root_table;
}

void entity_query_builder::push(const std::string &column_name)
{
  char str[4];
  snprintf(str, 4, "C%02d", ++column_index);
  const auto& col = entity_query_data_.columns.emplace_back(current_table_, column_name, str);
  column_ref_map_.insert({ { col.table_, col.name}, col });
}

[[nodiscard]] bool entity_query_builder::is_root_entity() const {
  return table_info_stack_.size() == 1;
}

void entity_query_builder::append_join(const column_key &left, const column_key &right)
{
  const auto left_it = column_ref_map_.find(left);
  const auto right_it = column_ref_map_.find(right);
  column left_col(left.table, left.name);
  column right_col(right.table, right.name);
  if (left_it != column_ref_map_.end()) {
    left_col = column(left_it->second.get().name, left_it->second.get().alias);
  }
  if (right_it != column_ref_map_.end()) {
    right_col = column(right_it->second.get().table_, right_it->second.get().name);
  }

  entity_query_data_.joins.push_back({right.table, make_condition(left_col == right_col)});
}

}