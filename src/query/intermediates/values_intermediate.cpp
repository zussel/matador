#include "matador/query/intermediates/values_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
fetchable_query values_intermediate::returning(const std::vector<column>& columns) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_returning_part>(columns));
  return {context};
}
}