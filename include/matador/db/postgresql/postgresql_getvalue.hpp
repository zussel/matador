#ifndef MATADOR_POSTGRESQL_GETVALUE_HPP
#define MATADOR_POSTGRESQL_GETVALUE_HPP

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"

#include <libpq-fe.h>
#include <cstring>

namespace matador {
namespace detail {

template < typename T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value && !std::is_same<T, char>::value>::type* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  if (PQgetisnull(res, (int)row, (int)col) == 1) {
    val = 0;
    return;
  }
  auto value = PQgetvalue(res, (int)row, (int)col);

  if (strlen(value) == 0) {
    return;
  }
  char *end;
  val = (T)strtoll(value, &end, 10);
}

template < typename T, typename std::enable_if<
std::is_integral<T>::value &&
std::is_unsigned<T>::value &&
!std::is_same<T, unsigned char>::value &&
!std::is_same<T, bool>::value
>::type* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  if (PQgetisnull(res, (int)row, (int)col) == 1) {
    val = 0;
    return;
  }
  auto value = PQgetvalue(res, (int)row, (int)col);

  if (strlen(value) == 0) {
    return;
  }
  char *end;
  val = (T)strtoull(value, &end, 10);
}

template < typename T, typename std::enable_if<std::is_same<T, bool>::value>::type* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  if (PQgetisnull(res, (int)row, (int)col) == 1) {
    val = false;
    return;
  }
  auto value = PQgetvalue(res, (int)row, (int)col);

  if (strlen(value) == 0) {
    return;
  }
  char *end;
  val = strtoul(value, &end, 10) > 0UL;
}

template < typename T, typename std::enable_if<std::is_same<T, char>::value>::type* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  if (PQgetisnull(res, (int)row, (int)col) == 1) {
    val = 0;
    return;
  }
  auto value = PQgetvalue(res, (int)row, (int)col);

  if (strlen(value) == 0) {
    return;
  }
  char *end;
  val = (char)strtol(value, &end, 10);
}

template < typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  if (PQgetisnull(res, (int)row, (int)col) == 1) {
    val = 0;
    return;
  }
  auto value = PQgetvalue(res, (int)row, (int)col);

  if (strlen(value) == 0) {
    return;
  }
  char *end;
  val = (T)strtof(value, &end);
}

template < typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  if (PQgetisnull(res, (int)row, (int)col) == 1) {
    val.clear();
    return;
  }
  if (PQgetisnull(res, (int)row, (int)col) == 1) {
    return;
  }
  val = PQgetvalue(res, (int)row, (int)col);
}

template < typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val, size_t)
{
  if (PQgetisnull(res, (int)row, (int)col) == 1) {
    val.clear();
    return;
  }
  val = PQgetvalue(res, (int)row, (int)col);
}

template < typename T, typename std::enable_if<std::is_same<T, matador::time>::value>::type* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  if (PQgetisnull(res, (int)row, (int)col) == 1) {
    return;
  }
  val = matador::time::parse(PQgetvalue(res, (int)row, (int)col), "%Y-%m-%d %T.%f");
}

template < typename T, typename std::enable_if<std::is_same<T, matador::date>::value>::type* = nullptr>
void get_value(PGresult *res, size_t row, size_t col, T &val)
{
  if (PQgetisnull(res, (int)row, (int)col) == 1) {
    return;
  }
  val.set(PQgetvalue(res, (int)row, (int)col), date_format::ISO8601);
}

void get_value(PGresult *res, size_t row, size_t col, char *val, size_t s);
void get_value(PGresult *res, size_t row, size_t col, unsigned char &val);

}
}

#endif //MATADOR_POSTGRESQL_GETVALUE_HPP
