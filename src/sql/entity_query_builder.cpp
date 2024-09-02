#include "matador/sql/entity_query_builder.hpp"

namespace matador::sql {

void entity_query_builder::on_primary_key(const char *id, std::string &, size_t)
{
  push(id);
  if (!is_root_entity()) {
    auto b = pk_.is_varchar();
    std::cout << "is matching primary key: " << std::boolalpha << b << "\n";
  }
}

void entity_query_builder::on_revision(const char *id, unsigned long long &/*rev*/)
{
  push(id);
}

void entity_query_builder::push(const std::string &column_name)
{
  char str[4];
  snprintf(str, 4, "C%02d", ++column_index);
  entity_query_data_.columns.emplace_back(table_info_stack_.top().name, column_name, str);
}

[[nodiscard]] bool entity_query_builder::is_root_entity() const {
  return table_info_stack_.size() == 1;
}

void entity_query_builder::append_join(const column &left, const column &right)
{
  entity_query_data_.joins.push_back({
                                       { right.table },
                                       make_condition(left == right)
                                     });
}

}