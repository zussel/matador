#include "matador/sql/query_part.hpp"

namespace matador::sql {

query_part::query_part(sql::dialect::token_t token)
: token_(token) {}

dialect::token_t query_part::token() const
{
  return token_;
}

}