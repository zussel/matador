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

detail::result_impl::size_type sqlite_prepared_result::reset_column_index() const
{
  return 0;
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, char &x)
{
  x = (char)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, short &x)
{
  x = (short)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, int &x)
{
  x = sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, long &x)
{
  x = (long)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, long long &x)
{
  x = (long)sqlite3_column_int64(stmt_, index);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, unsigned char &x)
{
  x = (unsigned char)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, unsigned short &x)
{
  x = (unsigned short)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, unsigned int &x)
{
  x = (unsigned int)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, unsigned long &x)
{
  x = (unsigned long)sqlite3_column_int(stmt_, index);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, unsigned long long &x)
{
  x = (unsigned long long)sqlite3_column_int64(stmt_, index);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, bool &x)
{
  x = sqlite3_column_int(stmt_, index) > 0;
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, float &x)
{
  x = (float)sqlite3_column_double(stmt_, index);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, double &x)
{
  x = sqlite3_column_double(stmt_, index);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, std::string &x)
{
  auto s = (size_t)sqlite3_column_bytes(stmt_, index);
  auto *text = (const char*)sqlite3_column_text(stmt_, index);
  x.assign(text, s);
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, std::string &x, size_t /*size*/)
{
  auto s = (size_t)sqlite3_column_bytes(stmt_, index);
  auto *text = (const char*)sqlite3_column_text(stmt_, index);
  if (s > 0) {
    x.assign(text, s);
  }
}

void sqlite_prepared_result::read_value(const char */*id*/, size_type index, char *x, size_t s)
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

void sqlite_prepared_result::read_value(const char *id, size_type index, matador::date &x)
{
  std::string val;
  read_value(id, index, val);
  x = matador::date::parse(val, date_format::ISO8601);
}

void sqlite_prepared_result::read_value(const char *id, size_type index, matador::time &x)
{
  std::string val;
  read_value(id, index, val);
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

void sqlite_prepared_result::close() {}

}

}
