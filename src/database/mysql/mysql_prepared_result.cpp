#include "database/mysql/mysql_prepared_result.hpp"

#include "object/object_atomizable.hpp"
#include "object/object_ptr.hpp"
#include "object/object.hpp"

#include "tools/varchar.hpp"

#include <ostream>
#include <cstring>

namespace oos {

namespace mysql {

mysql_prepared_result::mysql_prepared_result(MYSQL_STMT *s, MYSQL_BIND *b, int rs)
  : affected_rows_((size_type)mysql_stmt_affected_rows(s))
  , rows((size_type)mysql_stmt_num_rows(s))
  , fields_(mysql_stmt_field_count(s))
  , stmt(s)
  , result_size(rs)
  , bind_(b)
{
}

mysql_prepared_result::~mysql_prepared_result()
{
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
  // prepare bind array
  o->deserialize(*this);
  // bind result array to statement
  mysql_stmt_bind_result(stmt, bind_);
  // fetch data
  mysql_stmt_fetch(stmt);

  return rows-- > 0;
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
  get_column_value(bind_[result_index++], MYSQL_TYPE_TINY, x);
}

void mysql_prepared_result::read(const char *, short &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_SHORT, x);
}

void mysql_prepared_result::read(const char *, int &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_LONG, x);
}

void mysql_prepared_result::read(const char *, long &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_LONGLONG, x);
}

void mysql_prepared_result::read(const char *, unsigned char &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_TINY, x);
}

void mysql_prepared_result::read(const char *, unsigned short &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_SHORT, x);
}

void mysql_prepared_result::read(const char *, unsigned int &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_LONG, x);
}

void mysql_prepared_result::read(const char *, unsigned long &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_LONGLONG, x);
}

void mysql_prepared_result::read(const char *, bool &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_TINY, x);
}

void mysql_prepared_result::read(const char *, float &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_FLOAT, x);
}

void mysql_prepared_result::read(const char *, double &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_DOUBLE, x);
}

void mysql_prepared_result::read(const char *, char *x, int s)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_VAR_STRING, x, s);
}

void mysql_prepared_result::read(const char *, std::string &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_STRING, x);
}

void mysql_prepared_result::read(const char *, varchar_base &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_VAR_STRING, x);
}

void mysql_prepared_result::read(const char *, object_base_ptr &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_LONG, x);
}

void mysql_prepared_result::read(const char *, object_container &)
{}

void mysql_prepared_result::get_column_value(MYSQL_BIND &bind, enum_field_types, bool &value)
{
  value = (*static_cast<int*>(bind.buffer) > 0);
}

void mysql_prepared_result::get_column_value(MYSQL_BIND &bind, enum_field_types , char *x, int s)
{
#ifdef WIN32
  strncpy_s(x, s, (const char*)bind.buffer, s);
#else
  strncpy(x, (const char*)bind.buffer, s);
#endif
}

void mysql_prepared_result::get_column_value(MYSQL_BIND &bind, enum_field_types , std::string &value)
{
  // check real value
  unsigned long *len = bind.length;
  const char *buf = (const char*)bind.buffer;
  value.assign((const char*)bind.buffer/*, (std::string::size_type)bind.length*/);
}

void mysql_prepared_result::get_column_value(MYSQL_BIND &bind, enum_field_types , varchar_base &value)
{
  value.assign((const char*)bind.buffer/*, (std::string::size_type)bind.length*/);
}

void mysql_prepared_result::get_column_value(MYSQL_BIND &bind, enum_field_types , object_base_ptr &value)
{
  long id = *static_cast<long*>(bind.buffer);
  value.id(id);
//  value.assign((const char*)bind.buffer/*, (std::string::size_type)bind.length*/);
}

std::ostream& operator<<(std::ostream &out, const mysql_prepared_result &res)
{  
  out << "affected rows [" << res.affected_rows_ << "] size [" << res.rows << "]";
  return out;
}

}

}
