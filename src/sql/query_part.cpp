#include "matador/sql/query_part.hpp"

namespace matador::sql {

query_part::query_part(dialect_token token)
: token_(token) {}

dialect_token query_part::token() const
{
  return token_;
}

}