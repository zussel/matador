#include "matador/query/criteria/abstract_column_criteria.hpp"

namespace matador::query {
abstract_column_criteria::abstract_column_criteria(const column& col)
: column_(col) {}

const column& abstract_column_criteria::col() const {
  return column_;
}
}