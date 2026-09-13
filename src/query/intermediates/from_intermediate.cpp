#include "matador/query/intermediates/from_intermediate.hpp"

#include "matador/query/intermediates/join_intermediate.hpp"
#include "matador/query/intermediates/group_by_intermediate.hpp"
#include "matador/query/intermediates/order_by_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {

join_intermediate from_intermediate::join_left(const table &t) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_join_table_part>(t));
  return {context};
}

join_intermediate from_intermediate::join_left(const query_context &ctx) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_join_query_part>(ctx));
  return {context};
}

from_intermediate from_intermediate::join_left(join_data &data) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_join_table_part>(*data.join_table));
  context->parts.push_back(std::make_shared<internal::query_on_part>(std::move(data.condition)));
  return {context};
}

from_intermediate from_intermediate::join_left(std::vector<join_data> &data_vector) const {
  auto context = clone_context();
  for (auto &[join_table, condition] : data_vector) {
    context->parts.push_back(std::make_shared<internal::query_join_table_part>(*join_table));
    context->parts.push_back(std::make_shared<internal::query_on_part>(std::move(condition)));
  }
  return {context};
}

where_intermediate from_intermediate::where(std::unique_ptr<abstract_criteria>&& cond) const {
    return where_clause(std::move(cond));
}

where_intermediate from_intermediate::where_clause(std::unique_ptr<abstract_criteria> &&cond) const {
  if (cond) {
    auto context = clone_context();
    context->parts.push_back(std::make_shared<internal::query_where_part>(std::move(cond)));
    return {context};
  }
  return {context_};
}

group_by_intermediate from_intermediate::group_by(const column &column) const {
  return group_by({column});
}

group_by_intermediate from_intermediate::group_by(std::initializer_list<column> columns) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_group_by_part>(columns));
  return {context};
}

order_by_intermediate from_intermediate::order_by(const column &col) const {
  return order_by({col});
}

order_by_intermediate from_intermediate::order_by(std::initializer_list<column> columns) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_order_by_part>(columns));
  return {context};
}

}
