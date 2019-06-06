//
// Created by sascha on 06.06.19.
//

#ifndef MATADOR_POSTGRESQL_GETVALUE_HPP
#define MATADOR_POSTGRESQL_GETVALUE_HPP

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"

#include <libpq-fe.h>
#include <cstring>

namespace matador {
namespace detail {

//template < typename T >
//void get_value(PGresult *res, size_t row, size_t col, T &val);

template < typename T, typename std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value && !std::is_same<T, char>::value>* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  auto value = PQgetvalue(res, row, col);

  if (strlen(value) == 0) {
    return;
  }
  char *end;
  val = (T)strtol(value, &end, 10);
}

template < typename T, typename std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value && !std::is_same<T, bool>::value>* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  auto value = PQgetvalue(res, row, col);

  if (strlen(value) == 0) {
    return;
  }
  char *end;
  val = (T)strtoul(value, &end, 10);
}

template < typename T, typename std::enable_if_t<std::is_same<T, bool>::value>* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  auto value = PQgetvalue(res, row, col);

  if (strlen(value) == 0) {
    return;
  }
  char *end;
  val = (T)strtoul(value, &end, 10) > 0;
}

template < typename T, typename std::enable_if_t<std::is_same<T, char>::value>* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  auto value = PQgetvalue(res, row, col);

  if (strlen(value) > 1) {
    val = value[0];
  }
}

template < typename T, typename std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  auto value = PQgetvalue(res, row, col);

  if (strlen(value) == 0) {
    return;
  }
  char *end;
  val = (T)strtof(value, &end);
}

template < typename T, typename std::enable_if_t<std::is_base_of<varchar_base, T>::value>* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  val.assign(PQgetvalue(res, row, col));
}

template < typename T, typename std::enable_if_t<std::is_same<T, std::string>::value>* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  val = PQgetvalue(res, row, col);
}

template < typename T, typename std::enable_if_t<std::is_same<T, matador::time>::value>* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  val = matador::time::parse(PQgetvalue(res, row, col), "%Y-%m-%d %T.%f");
}

template < typename T, typename std::enable_if_t<std::is_same<T, matador::date>::value>* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  val.set(PQgetvalue(res, row, col), date_format::ISO8601);
}

void get_value(PGresult *res, size_t row, size_t col, char *val, size_t s);

}
}

#endif //MATADOR_POSTGRESQL_GETVALUE_HPP
