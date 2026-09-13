#include "matador/query/intermediates/insert_intermediate.hpp"

#include "matador/query/intermediates/into_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
insert_intermediate::insert_intermediate() {
  context_->parts.push_back(std::make_unique<internal::query_insert_part>());
}

into_intermediate insert_intermediate::into(const table &tab, const std::initializer_list<column> columns) const {
  return into(tab, std::vector<column>{columns});
}

into_intermediate insert_intermediate::into(const table &tab, const std::vector<column> &columns) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_into_part>(tab, columns));
  return {context};
}

into_intermediate insert_intermediate::into(const table &tab, const std::vector<std::string> &column_names) const {
  std::vector<column> columns;
  columns.reserve(column_names.size());
  for (const auto &col_name: column_names) {
    columns.emplace_back(column::make_plain(col_name));
  }
  return into(tab, columns);
}

into_intermediate insert_intermediate::into(const table &tab) const {
  return into(tab, tab.columns());
}
}
