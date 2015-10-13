#include "sqlite_prepared_result.hpp"

#include "tools/date.hpp"
#include "tools/time.hpp"
#include "tools/varchar.hpp"

#include "object/object_ptr.hpp"

#include <cstring>

#include <sqlite3.h>

namespace oos {

namespace sqlite {

sqlite_prepared_result::sqlite_prepared_result(sqlite3_stmt *stmt, int ret, std::shared_ptr<oos::object_base_producer> producer)
  : result_impl(producer)
  , ret_(ret)
  , first_(true)
  , affected_rows_(0)
  , rows(0)
  , fields_(0)
  , stmt_(stmt)
{
}

sqlite_prepared_result::~sqlite_prepared_result()
{
}

const char* sqlite_prepared_result::column(size_type ) const
{
  return 0;
}

bool sqlite_prepared_result::fetch()
{
  if (!first_) {
    // get next row
    ret_ = sqlite3_step(stmt_);
  } else {
    first_ = false;
  }
  if (ret_ == SQLITE_DONE || ret_ == SQLITE_OK) {
    // no further row available
    return false;
  }

  return true;
}

bool sqlite_prepared_result::fetch(serializable *o)
{
  if (!fetch()) {
    return false;
  }
  
  get(o);

  return true;
}

sqlite_prepared_result::size_type sqlite_prepared_result::affected_rows() const
{
  sqlite3 *db = sqlite3_db_handle(stmt_);
  return (size_type)sqlite3_changes(db);
//  return affected_rows_;
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

void sqlite_prepared_result::read(const char *, char &x)
{
  x = (char)sqlite3_column_int(stmt_, result_index++);
}

void sqlite_prepared_result::read(const char *, short &x)
{
  x = (short)sqlite3_column_int(stmt_, result_index++);
}

void sqlite_prepared_result::read(const char *, int &x)
{
  x = (int)sqlite3_column_int(stmt_, result_index++);
}

void sqlite_prepared_result::read(const char *, long &x)
{
  x = (long)sqlite3_column_int(stmt_, result_index++);
}

void sqlite_prepared_result::read(const char *, unsigned char &x)
{
  x = (unsigned char)sqlite3_column_int(stmt_, result_index++);
}

void sqlite_prepared_result::read(const char *, unsigned short &x)
{
  x = (unsigned short)sqlite3_column_int(stmt_, result_index++);
}

void sqlite_prepared_result::read(const char *, unsigned int &x)
{
  x = (unsigned int)sqlite3_column_int(stmt_, result_index++);
}

void sqlite_prepared_result::read(const char *, unsigned long &x)
{
  x = (unsigned long)sqlite3_column_int(stmt_, result_index++);
}

void sqlite_prepared_result::read(const char *, bool &x)
{
  x = sqlite3_column_int(stmt_, result_index++) > 0;
}

void sqlite_prepared_result::read(const char *, float &x)
{
  x = (float)sqlite3_column_double(stmt_, result_index++);
}

void sqlite_prepared_result::read(const char *, double &x)
{
  x = sqlite3_column_double(stmt_, result_index++);
}

void sqlite_prepared_result::read(const char *, std::string &x)
{
  int s = sqlite3_column_bytes(stmt_, result_index);
  const char *text = (const char*)sqlite3_column_text(stmt_, result_index++);
  x.assign(text, s);
}

void sqlite_prepared_result::read(const char *, varchar_base &x)
{
  int s = sqlite3_column_bytes(stmt_, result_index);
  const char *text = (const char*)sqlite3_column_text(stmt_, result_index++);
  if (s == 0) {
  } else {
    x.assign(text, s);
  }
}

void sqlite_prepared_result::read(const char *, char *x, int s)
{
  int size = sqlite3_column_bytes(stmt_, result_index);
  if (size < s) {
#ifdef WIN32
    strncpy_s(x, size, (const char*)sqlite3_column_text(stmt_, result_index++), s);
#else
    strncpy(x, (const char*)sqlite3_column_text(stmt_, result_index++), size);
#endif
    x[size] = '\0';
  } else {
#ifdef WIN32
    strncpy_s(x, size, (const char*)sqlite3_column_text(stmt_, result_index++), s - 1);
#else
    strncpy(x, (const char*)sqlite3_column_text(stmt_, result_index++), s - 1);
#endif
    x[s] = '\0';
  }
}

void sqlite_prepared_result::read(const char *id, oos::date &x)
{
  double val = 0;
  read(id, val);
  x.set(static_cast<int>(val));
}

void sqlite_prepared_result::read(const char *id, oos::time &x)
{
  std::string val;
  read(id, val);
  x = oos::time::parse(val, "%F %T.%f");
}

void sqlite_prepared_result::read(const char *id, object_base_ptr &x)
{
  read_foreign_object(id, x);
}

void sqlite_prepared_result::read(const char *, object_container &) { }

void sqlite_prepared_result::read(const char *id, basic_identifier &x)
{
  x.deserialize(id, *this);
}
}

}
