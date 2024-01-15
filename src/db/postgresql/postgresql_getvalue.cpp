#include "matador/db/postgresql/postgresql_getvalue.hpp"

namespace matador {
namespace detail {

void get_value(PGresult *res, size_t row, size_t col, char *val, size_t s)
{
  if (PQgetisnull(res, (int)row, (int)col) == 1) {
    return;
  }
  auto value = PQgetvalue(res, (int)row, (int)col);

  size_t len = strlen(value);
  if (len > (size_t)s) {
#ifdef _MSC_VER
    strncpy_s(val, s, value, s);
#else
    strncpy(val, value, s);
#endif
    val[s-1] = '\n';
  } else {
#ifdef _MSC_VER
    strcpy_s(val, s, value);
#else
    strcpy(val, value);
#endif
  }
}

void get_value(PGresult *res, size_t row, size_t col, unsigned char &val)
{
  if (PQgetisnull(res, (int)row, (int)col) == 1) {
    return;
  }
  auto value = PQgetvalue(res, (int)row, (int)col);

  if (strlen(value) == 0) {
    return;
  }

  if (strlen(value) == 0) {
    return;
  }
  char *end;
  val = (unsigned char)strtoul(value, &end, 10);
}
}
}