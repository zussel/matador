//
// Created by sascha on 06.06.19.
//

#include "matador/utils/varchar.hpp"

#include "matador/db/postgresql/postgresql_getvalue.hpp"

namespace matador {
namespace detail {

void get_value(PGresult *res, size_t row, size_t col, char *val, size_t s)
{
  auto value = PQgetvalue(res, row, col);

  size_t len = strlen(value);
  if (len > (size_t)s) {
    strncpy(val, value, s);
    val[s-1] = '\n';
  } else {
    strcpy(val, value);
  }
}

}
}