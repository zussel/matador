#include "matador/query/intermediates/execute_limit_intermediate.hpp"
#include "matador/query/intermediates/execute_offset_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
execute_offset_intermediate execute_limit_intermediate::offset(size_t offset) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_offset_part>(offset));
  return {context};
}
} // namespace matador::query
