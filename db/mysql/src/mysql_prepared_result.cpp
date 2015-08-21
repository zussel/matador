#include "mysql_prepared_result.hpp"
#include "mysql_column_binder.hpp"
#include "mysql_exception.hpp"
#include "mysql_column_binder.hpp"

#include "tools/varchar.hpp"

#include "object/serializable.hpp"
#include "object/object_ptr.hpp"

#include <cstring>

namespace oos {

namespace mysql {

mysql_prepared_result::mysql_prepared_result(MYSQL_STMT *s, int rs)
  : column_index_(0)
  , affected_rows_((size_type)mysql_stmt_affected_rows(s))
  , rows((size_type)mysql_stmt_num_rows(s))
  , fields_(mysql_stmt_field_count(s))
  , stmt(s)
  , result_size(rs)
  , bind_(new MYSQL_BIND[rs])
  , info_(new result_info[rs])
{
    memset(bind_, 0, rs * sizeof(MYSQL_BIND));
    memset(info_, 0, rs * sizeof(result_info));
}

mysql_prepared_result::~mysql_prepared_result()
{
  delete [] bind_;
  for (int i = 0; i < result_size; ++i) {
    if (info_[i].buffer != 0) {
      delete [] info_[i].buffer;
    }
  }
  delete [] info_;
}

const char* mysql_prepared_result::column(size_type ) const
{
  return "not implemented";
}

bool mysql_prepared_result::fetch()
{
  // get next row
  int ret = mysql_stmt_fetch(stmt);
  if (ret == MYSQL_DATA_TRUNCATED) {
  }
/*
      log_debug("mysql_stmt_fetch(" << stmt << ')');
      int ret = mysql_stmt_fetch(stmt);

      if (ret == MYSQL_DATA_TRUNCATED)
      {
        // fetch column data where truncated
        MYSQL_FIELD* fields = mysqlStatement->getFields();
        for (unsigned n = 0; n < row->getSize(); ++n)
        {
          if (*row->getMysqlBind()[n].length > row->getMysqlBind()[n].buffer_length)
          {
            // actual length was longer than buffer_length, so this column is truncated
            fields[n].length = *row->getMysqlBind()[n].length;
            row->initOutBuffer(n, fields[n]);

            log_debug("mysql_stmt_fetch_column(" << stmt << ", BIND, " << n
                << ", 0) with " << fields[n].length << " bytes");
            if (mysql_stmt_fetch_column(stmt, row->getMysqlBind() + n, n, 0) != 0)
              throw MysqlStmtError("mysql_stmt_fetch_column", stmt);
          }
        }
      }
      else if (ret == MYSQL_NO_DATA)
      {
        log_debug("MYSQL_NO_DATA");
        row = 0;
        return Row();
      }
      else if (ret == 1)
        throw MysqlStmtError("mysql_stmt_fetch", stmt);

      return Row(&*row);
  */
  return rows-- > 0;
}

bool mysql_prepared_result::fetch(serializable *o)
{
  // reset result column index
  column_index_ = 0;
  result_index = 0;
  // prepare result array
  mysql_column_binder binder;
  binder.bind(o, stmt, bind_);

  // fetch data
  int ret = mysql_stmt_fetch(stmt);
  if (ret == MYSQL_NO_DATA) {
    return false;
  } else if (ret == 1) {
    throw_stmt_error(ret, stmt, "mysql", "");
  } else {
//  if (ret == MYSQL_DATA_TRUNCATED) {
    // load data from database
    o->serialize(*this);
//  }
  }

  return true;

//  return rows-- > 0;
}

mysql_prepared_result::size_type mysql_prepared_result::affected_rows() const
{
  return affected_rows_;
}

mysql_prepared_result::size_type mysql_prepared_result::result_rows() const
{
  return rows;
}

mysql_prepared_result::size_type mysql_prepared_result::fields() const
{
  return fields_;
}

int mysql_prepared_result::transform_index(int index) const
{
  return index;
}

void mysql_prepared_result::read(const char *id, char &x)
{
 ++column_index_;
}

void mysql_prepared_result::read(const char *id, short &x)
{
 ++column_index_;
}

void mysql_prepared_result::read(const char *id, int &x)
{
 ++column_index_;
}

void mysql_prepared_result::read(const char *id, long &x)
{
 ++column_index_;
}

void mysql_prepared_result::read(const char *id, unsigned char &x)
{
 ++column_index_;
}

void mysql_prepared_result::read(const char *id, unsigned short &x)
{
 ++column_index_;
}

void mysql_prepared_result::read(const char *id, unsigned int &x)
{
 ++column_index_;
}

void mysql_prepared_result::read(const char *id, unsigned long &x)
{
 ++column_index_;
}

void mysql_prepared_result::read(const char *id, bool &x)
{
 ++column_index_;
}

void mysql_prepared_result::read(const char *id, float &x)
{
 ++column_index_;
}

void mysql_prepared_result::read(const char *id, double &x)
{
 ++column_index_;
}

void mysql_prepared_result::read(const char *id, char *x, int s)
{
 ++column_index_;
}

void mysql_prepared_result::read(const char *, oos::date &x)
{
  if (info_[column_index_].length > 0) {
    MYSQL_TIME *mtt = (MYSQL_TIME*)info_[column_index_].buffer;
    x.set(mtt->day, mtt->month, mtt->year);
  }
  ++column_index_;
}

void mysql_prepared_result::read(const char *, oos::time &x)
{
  if (info_[column_index_].length > 0) {
    MYSQL_TIME *mtt = (MYSQL_TIME*)info_[column_index_].buffer;
    x.set(mtt->year, mtt->month, mtt->day, mtt->hour, mtt->minute, mtt->second, mtt->second_part / 1000);
  }
  ++column_index_;
}

void mysql_prepared_result::read(const char *, std::string &x)
{
  if (info_[column_index_].length > 0) {
    bind_[column_index_].buffer = new char[info_[column_index_].length];
    bind_[column_index_].buffer_length = info_[column_index_].length;
    if (mysql_stmt_fetch_column(stmt_, &bind_[column_index_], column_index_, 0) != 0) {
      // an error occured
    } else {
      char *data = (char*)bind_[column_index_].buffer;
      unsigned long len = bind_[column_index_].buffer_length;
      x.assign(data, len);
    }
    delete [] (char*)bind_[column_index_].buffer;
    bind_[column_index_].buffer = 0;
  }
  ++column_index_;
}

void mysql_prepared_result::read(const char *, varchar_base &x)
{
  char *data = (char*)bind_[column_index_].buffer;
//  unsigned long len = bind_[column_index_].buffer_length;
  unsigned long len = info_[column_index_].length;
  x.assign(data, len);
  ++column_index_;
}

void mysql_prepared_result::read(const char *, object_base_ptr &x)
{
  long id = *static_cast<long*>(bind_[column_index_].buffer);
  x.id(id);
}

void mysql_prepared_result::read(const char *id, object_container &x)
{
  ++column_index_;
}

void mysql_prepared_result::read(const char *id, primary_key_base &x)
{
  ++column_index_;
}

void mysql_prepared_result::read(const char *, char *, int )
{
  ++column_index_;
}

std::ostream& operator<<(std::ostream &out, const mysql_prepared_result &res)
{  
  out << "affected rows [" << res.affected_rows_ << "] size [" << res.rows << "]";
  return out;
}

}

}
