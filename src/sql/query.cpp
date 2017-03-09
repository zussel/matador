//
// Created by sascha on 5/24/16.
//

#include "matador/sql/query.hpp"

namespace matador {

query<row> select(columns cols)
{
  query<row> q;
  return q.select(cols);
}

}