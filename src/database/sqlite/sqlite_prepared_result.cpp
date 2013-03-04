#include "database/sqlite/sqlite_prepared_result.hpp"
#include "object/object.hpp"

#include <ostream>

#include <sqlite3.h>

namespace oos {

namespace sqlite {

sqlite_prepared_result::sqlite_prepared_result(sqlite3_stmt *stmt, int ret)
  : ret_(ret)
  , first_(true)
  , affected_rows_(0)
  , rows(0)
  , fields_(0)
  , stmt_(stmt)
  , result_index(0)
  , result_size(0)
{
}

sqlite_prepared_result::~sqlite_prepared_result()
{
}

void sqlite_prepared_result::get(object_atomizable *o)
{
  result_index = 0;
  o->deserialize(*this);
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

sqlite_prepared_result::size_type sqlite_prepared_result::affected_rows() const
{
  return affected_rows_;
}

sqlite_prepared_result::size_type sqlite_prepared_result::result_rows() const
{
  return rows;
}

sqlite_prepared_result::size_type sqlite_prepared_result::fields() const
{
  return fields_;
}

void sqlite_prepared_result::read(const char *, long &x)
{
  x = (long)sqlite3_column_int(stmt_, result_index++);
}
void sqlite_prepared_result::read(const char *, double &x)
{
  x = sqlite3_column_double(stmt_, result_index++);
}
void sqlite_prepared_result::read(const char *, std::string &x)
{
  int s = sqlite3_column_bytes(stmt_, result_index);
  x.assign((const char*)sqlite3_column_text(stmt_, result_index++), s);
}

std::ostream& operator<<(std::ostream &out, const sqlite_prepared_result &res)
{  
  out << "affected rows [" << res.affected_rows_ << "] size [" << res.rows << "]";
  return out;
}

}

}
