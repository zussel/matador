#include "matador/query/intermediates/delete_intermediate.hpp"

#include "matador/query/intermediates/delete_from_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
delete_intermediate::delete_intermediate() {
  context_->parts.push_back(std::make_unique<internal::query_delete_part>());
}

delete_from_intermediate delete_intermediate::from(const table &tab) const {
  auto context = clone_context();
  context->parts.push_back(std::make_shared<internal::query_delete_from_part>(tab));
  return {context};
}
}
