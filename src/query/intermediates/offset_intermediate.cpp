#include "matador/query/intermediates/offset_intermediate.hpp"
#include "matador/query/intermediates/limit_intermediate.hpp"
#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
limit_intermediate offset_intermediate::limit(size_t limit) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_limit_part>(limit));
  return {context};
}
}
