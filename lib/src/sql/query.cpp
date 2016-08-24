//
// Created by sascha on 5/24/16.
//

#include "sql/query.hpp"

namespace oos {

query<row> select(columns cols)
{
  query<row> q;
  return q.select(cols);
}

}