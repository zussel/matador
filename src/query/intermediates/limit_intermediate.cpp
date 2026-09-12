#include "matador/query/intermediates/limit_intermediate.hpp"
#include "matador/query/intermediates/offset_intermediate.hpp"
#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
offset_intermediate limit_intermediate::offset(size_t offset) {
  context_->parts.push_back(std::make_unique<internal::query_offset_part>(offset));
  return {context_};
}
}
