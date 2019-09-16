#include "matador/db/sqlite/sqlite_prepared_result.hpp"
#include "matador/db/sqlite/sqlite_exception.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"
#include "matador/utils/basic_identifier.hpp"

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

void sqlite_prepared_result::serialize(const char *, char &x)
{
  x = (char)sqlite3_column_int(stmt_, result_index_++);
}

void sqlite_prepared_result::serialize(const char *, short &x)
{
  x = (short)sqlite3_column_int(stmt_, result_index_++);
}

void sqlite_prepared_result::serialize(const char *, int &x)
{
  x = sqlite3_column_int(stmt_, result_index_++);
}

void sqlite_prepared_result::serialize(const char *, long &x)
{
  x = (long)sqlite3_column_int(stmt_, result_index_++);
}

void sqlite_prepared_result::serialize(const char *, unsigned char &x)
{
  x = (unsigned char)sqlite3_column_int(stmt_, result_index_++);
}

void sqlite_prepared_result::serialize(const char *, unsigned short &x)
{
  x = (unsigned short)sqlite3_column_int(stmt_, result_index_++);
}

void sqlite_prepared_result::serialize(const char *, unsigned int &x)
{
  x = (unsigned int)sqlite3_column_int(stmt_, result_index_++);
}

void sqlite_prepared_result::serialize(const char *, unsigned long &x)
{
  x = (unsigned long)sqlite3_column_int(stmt_, result_index_++);
}

void sqlite_prepared_result::serialize(const char *, bool &x)
{
  x = sqlite3_column_int(stmt_, result_index_++) > 0;
}

void sqlite_prepared_result::serialize(const char *, float &x)
{
  x = (float)sqlite3_column_double(stmt_, result_index_++);
}

void sqlite_prepared_result::serialize(const char *, double &x)
{
  x = sqlite3_column_double(stmt_, result_index_++);
}

void sqlite_prepared_result::serialize(const char *, std::string &x)
{
  auto s = (size_t)sqlite3_column_bytes(stmt_, result_index_);
  auto *text = (const char*)sqlite3_column_text(stmt_, result_index_++);
  x.assign(text, s);
}

void sqlite_prepared_result::serialize(const char *, std::string &x, size_t )
{
  auto s = (size_t)sqlite3_column_bytes(stmt_, result_index_);
  auto *text = (const char*)sqlite3_column_text(stmt_, result_index_++);
  if (s > 0) {
    x.assign(text, s);
  }
}

void sqlite_prepared_result::serialize(const char *, char *x, size_t s)
{
  auto size = (size_t)sqlite3_column_bytes(stmt_, result_index_);
  if (size < s) {
#ifdef _MSC_VER
    auto val = sqlite3_column_text(stmt_, result_index_++);
    strncpy_s(x, s, (const char*)val, size);
#else
    strncpy(x, (const char*)sqlite3_column_text(stmt_, result_index_++), size);
#endif
    x[size] = '\0';
  } else {
#ifdef _MSC_VER
    strncpy_s(x, size, (const char*)sqlite3_column_text(stmt_, result_index_++), s - 1);
#else
    strncpy(x, (const char*)sqlite3_column_text(stmt_, result_index_++), s - 1);
#endif
    x[s] = '\0';
  }
}

void sqlite_prepared_result::serialize(const char *id, matador::date &x)
{
  std::string val;
  serialize(id, val);
  x = matador::date::parse(val, date_format::ISO8601);
}

void sqlite_prepared_result::serialize(const char *id, matador::time &x)
{
  std::string val;
  serialize(id, val);
  x = matador::time::parse(val, "%Y-%m-%dT%T.%f");
}

void sqlite_prepared_result::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  read_foreign_object(id, x);
}

void sqlite_prepared_result::serialize(const char *id, basic_identifier &x)
{
  x.serialize(id, *this);
}

bool sqlite_prepared_result::prepare_fetch()
{
  if (!first_) {
    // get next row
    ret_ = sqlite3_step(stmt_);
    if (ret_ != SQLITE_DONE && ret_ != SQLITE_ROW) {
      sqlite3 *db = sqlite3_db_handle(stmt_);
      throw_error(ret_, db, "sqlite3_step");
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
