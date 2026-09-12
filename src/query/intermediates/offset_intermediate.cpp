#include "matador/query/intermediates/offset_intermediate.hpp"
#include "matador/query/intermediates/limit_intermediate.hpp"
#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
limit_intermediate offset_intermediate::limit(size_t limit) {
  context_->parts.push_back(std::make_unique<internal::query_limit_part>(limit));
  return {context_};
}
}
