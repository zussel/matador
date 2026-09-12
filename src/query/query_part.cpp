#include "matador/query/query_part.hpp"

namespace matador::query {
query_part::query_part(const dialect_token token)
: token_(token) {}

dialect_token query_part::token() const {
  return token_;
}
}
