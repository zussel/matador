#include "mysql_prepared_result.hpp"
#include "mysql_column_fetcher.hpp"
#include "mysql_exception.hpp"

#include "object/object_atomizable.hpp"
#include "object/object_ptr.hpp"
#include "object/object.hpp"

#include "tools/varchar.hpp"

#include <ostream>
#include <cstring>
#include <fakemysql.h>

namespace oos {

namespace mysql {

mysql_prepared_result::mysql_prepared_result(MYSQL_STMT *s, int rs)
  : affected_rows_((size_type)mysql_stmt_affected_rows(s))
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

bool mysql_prepared_result::fetch(object *o)
{
  // reset result column index
  result_index = 0;
  // prepare result array
  o->deserialize(*this);
  // bind result array to statement
  mysql_stmt_bind_result(stmt, bind_);
  // fetch data
  int ret = mysql_stmt_fetch(stmt);
  if (ret == MYSQL_NO_DATA) {
    return false;
  } else if (ret == 1) {
    throw_stmt_error(ret, stmt, "mysql", "");
  } else {
//  if (ret == MYSQL_DATA_TRUNCATED) {
    // load data from database
    mysql_column_fetcher fetcher(stmt, bind_, info_);
    fetcher.fetch(o);
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

void mysql_prepared_result::read(const char *, char &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_TINY, x);
}

void mysql_prepared_result::read(const char *, short &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_SHORT, x);
}

void mysql_prepared_result::read(const char *, int &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_LONG, x);
}

void mysql_prepared_result::read(const char *, long &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_LONGLONG, x);
}

void mysql_prepared_result::read(const char *, unsigned char &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_TINY, x);
}

void mysql_prepared_result::read(const char *, unsigned short &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_SHORT, x);
}

void mysql_prepared_result::read(const char *, unsigned int &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_LONG, x);
}

void mysql_prepared_result::read(const char *, unsigned long &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_LONGLONG, x);
}

void mysql_prepared_result::read(const char *, bool &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_TINY, x);
}

void mysql_prepared_result::read(const char *, float &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_FLOAT, x);
}

void mysql_prepared_result::read(const char *, double &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_DOUBLE, x);
}

void mysql_prepared_result::read(const char *, char *x, int s)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_VAR_STRING, x, s);
}

void mysql_prepared_result::read(const char *, std::string &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_STRING, x);
}

void mysql_prepared_result::read(const char *, oos::date &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_DATE, x);
}

void mysql_prepared_result::read(const char *, oos::time &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_TIMESTAMP, x);
}

void mysql_prepared_result::read(const char *, varchar_base &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_VAR_STRING, x);
}

void mysql_prepared_result::read(const char *, object_base_ptr &x)
{
  prepare_bind_column(result_index++, MYSQL_TYPE_LONG, x);
}

void mysql_prepared_result::read(const char *, object_container &)
{}


void mysql_prepared_result::read(const char *id, primary_key_base &x)
{
  x.deserialize(id, *this);
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, oos::date &)
{
  if (info_[index].buffer == 0) {
    size_t s = sizeof(MYSQL_TIME);
    info_[index].buffer = (char*)new MYSQL_TIME;
    info_[index].buffer_length = s;
  }
  bind_[index].buffer_type = type;
  bind_[index].buffer = info_[index].buffer;
  bind_[index].buffer_length = info_[index].buffer_length;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, oos::time &)
{
  if (info_[index].buffer == 0) {
    size_t s = sizeof(MYSQL_TIME);
    info_[index].buffer = (char*)new MYSQL_TIME;
    info_[index].buffer_length = s;
  }
  bind_[index].buffer_type = type;
  bind_[index].buffer = info_[index].buffer;
  bind_[index].buffer_length = info_[index].buffer_length;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, std::string & /*value*/)
{
  bind_[index].buffer_type = type;
  bind_[index].buffer = 0;
  bind_[index].buffer_length = 0;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, char *x, int s)
{
  bind_[index].buffer_type = type;
  bind_[index].buffer= x;
  bind_[index].buffer_length = s;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, varchar_base &x)
{
  if (info_[index].buffer == 0) {
    info_[index].buffer = new char[x.capacity()];
    memset(info_[index].buffer, 0, x.capacity());
    info_[index].buffer_length = x.capacity();
  }
  bind_[index].buffer_type = type;
  bind_[index].buffer = info_[index].buffer;
  bind_[index].buffer_length = info_[index].buffer_length;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, object_base_ptr &)
{
  if (info_[index].buffer == 0) {
    info_[index].buffer = new char[sizeof(long)];
    memset(info_[index].buffer, 0, sizeof(long));
    info_[index].buffer_length = sizeof(long);
  }

  bind_[index].buffer_type = type;
  bind_[index].buffer = info_[index].buffer;
  bind_[index].buffer_length = info_[index].buffer_length;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

std::ostream& operator<<(std::ostream &out, const mysql_prepared_result &res)
{  
  out << "affected rows [" << res.affected_rows_ << "] size [" << res.rows << "]";
  return out;
}

}

}
