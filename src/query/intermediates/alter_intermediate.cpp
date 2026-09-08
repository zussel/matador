#include "matador/query/intermediates/alter_intermediate.hpp"

#include "matador/query/internal/query_parts.hpp"
#include "matador/query/query_data.hpp"

namespace matador::query {
alter_intermediate::alter_intermediate() {
  context_->parts.push_back(std::make_unique<internal::query_alter_part>());
}

alter_table_intermediate alter_intermediate::table(const class table& tab) const {
  context_->parts.push_back(std::make_unique<internal::query_alter_table_part>(tab));
  return {context_};
}
}