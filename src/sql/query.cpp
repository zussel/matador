#include "matador/sql/query.hpp"

namespace matador {

query<row> select(columns cols)
{
  query<row> q;
  return q.select(cols);
}

}