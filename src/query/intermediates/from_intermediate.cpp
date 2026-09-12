#include "matador/query/intermediates/from_intermediate.hpp"

#include "matador/query/intermediates/join_intermediate.hpp"
#include "matador/query/intermediates/group_by_intermediate.hpp"
#include "matador/query/intermediates/order_by_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {

join_intermediate from_intermediate::join_left(const table &t) {
  context_->parts.push_back(std::make_unique<internal::query_join_table_part>(t));
  return {context_};
}

join_intermediate from_intermediate::join_left(const query_context &ctx) {
  context_->parts.push_back(std::make_unique<internal::query_join_query_part>(ctx));
  return {context_};
}

from_intermediate from_intermediate::join_left(join_data &data) {
  context_->parts.push_back(std::make_unique<internal::query_join_table_part>(*data.join_table));
  context_->parts.push_back(std::make_unique<internal::query_on_part>(std::move(data.condition)));
  return {context_};
}

from_intermediate from_intermediate::join_left(std::vector<join_data> &data_vector) {
  for (auto &[join_table, condition] : data_vector) {
    context_->parts.push_back(std::make_unique<internal::query_join_table_part>(*join_table));
    context_->parts.push_back(std::make_unique<internal::query_on_part>(std::move(condition)));
  }
  return {context_};
}

where_intermediate from_intermediate::where( std::unique_ptr<abstract_criteria>&& cond ) {
    return where_clause(std::move(cond));
}

where_intermediate from_intermediate::where_clause(std::unique_ptr<abstract_criteria> &&cond) {
  if (cond) {
    context_->parts.push_back(std::make_unique<internal::query_where_part>(std::move(cond)));
  }
  return {context_};
}

group_by_intermediate from_intermediate::group_by(const column &column) {
  return group_by({column});
}

group_by_intermediate from_intermediate::group_by(std::initializer_list<column> columns) {
  context_->parts.push_back(std::make_unique<internal::query_group_by_part>(columns));
  return {context_};
}

order_by_intermediate from_intermediate::order_by(const column &col) {
  return order_by({col});
}

order_by_intermediate from_intermediate::order_by(std::initializer_list<column> columns) {
  context_->parts.push_back(std::make_unique<internal::query_order_by_part>(columns));
  return {context_};
}

}
