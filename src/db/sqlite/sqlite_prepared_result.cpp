#include "matador/db/sqlite/sqlite_prepared_result.hpp"
#include "matador/db/sqlite/sqlite_exception.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"

#include <cstring>

#include <sqlite3.h>

namespace matador {

namespace sqlite {

sqlite_prepared_result::sqlite_prepared_result(sqlite3_stmt *stmt, int ret)
  : ret_(ret)
  , first_(true)
  , rows(0)
  , fields_(0)
  , stmt_(stmt)
{
}

const char* sqlite_prepared_result::column(size_type c) const
{
  return (const char*)sqlite3_column_text(stmt_, c);
}

bool sqlite_prepared_result::fetch()
{
  return prepare_fetch();
}

sqlite_prepared_result::size_type sqlite_prepared_result::affected_rows() const
{
  sqlite3 *db = sqlite3_db_handle(stmt_);
  return (size_type)sqlite3_changes(db);
}

sqlite_prepared_result::size_type sqlite_prepared_result::result_rows() const
{
  return rows;
}

sqlite_prepared_result::size_type sqlite_prepared_result::fields() const
{
  return fields_;
}

int sqlite_prepared_result::transform_index(int index) const
{
  return index;
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, char &x)
{
  x = (char)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, short &x)
{
  x = (short)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, int &x)
{
  x = sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, long &x)
{
  x = (long)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, long long &x)
{
  x = (long)sqlite3_column_int64(stmt_, index);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, unsigned char &x)
{
  x = (unsigned char)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, unsigned short &x)
{
  x = (unsigned short)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, unsigned int &x)
{
  x = (unsigned int)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, unsigned long &x)
{
  x = (unsigned long)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, unsigned long long &x)
{
  x = (unsigned long long)sqlite3_column_int64(stmt_, index);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, bool &x)
{
  x = sqlite3_column_int(stmt_, index) > 0;
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, float &x)
{
  x = (float)sqlite3_column_double(stmt_, index);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, double &x)
{
  x = sqlite3_column_double(stmt_, index);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, std::string &x)
{
  auto s = (size_t)sqlite3_column_bytes(stmt_, result_index_);
  auto *text = (const char*)sqlite3_column_text(stmt_, index);
  x.assign(text, s);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, std::string &x, long /*size*/)
{
  auto s = (size_t)sqlite3_column_bytes(stmt_, result_index_);
  auto *text = (const char*)sqlite3_column_text(stmt_, index);
  if (s > 0) {
    x.assign(text, s);
  }
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, char *x, long s)
{
  auto size = (size_t)sqlite3_column_bytes(stmt_, index);
  if (size < s) {
#ifdef _MSC_VER
    strncpy_s(x, s, (const char*)sqlite3_column_text(stmt_, index), size);
#else
    strncpy(x, (const char*)sqlite3_column_text(stmt_, index), size);
#endif
    x[size] = '\0';
  } else {
#ifdef _MSC_VER
    strncpy_s(x, s, (const char*)sqlite3_column_text(stmt_, index), size - 1);
#else
    strncpy(x, (const char*)sqlite3_column_text(stmt_, index), size - 1);
#endif
    x[s] = '\0';
  }
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, matador::date &x)
{
  std::string val;
  on_attribute(id, val);
  x = matador::date::parse(val, date_format::ISO8601);
}

void sqlite_prepared_result::read_value(const char *id, int index, int row, matador::time &x)
{
  std::string val;
  on_attribute(id, val);
  x = matador::time::parse(val, "%Y-%m-%dT%T.%f");
}

bool sqlite_prepared_result::prepare_fetch()
{
  if (!first_) {
    // get next row
    ret_ = sqlite3_step(stmt_);
    if (ret_ != SQLITE_DONE && ret_ != SQLITE_ROW) {
      sqlite3 *db = sqlite3_db_handle(stmt_);
      throw_database_error(ret_, db, "sqlite3_step");
    }
  } else {
    first_ = false;
  }
  return ret_ == SQLITE_ROW;
}

bool sqlite_prepared_result::finalize_fetch()
{
  return true;
}

}

}
