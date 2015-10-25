#include "mysql_prepared_result.hpp"
#include "mysql_column_binder.hpp"
#include "mysql_exception.hpp"
#include "mysql_column_binder.hpp"
#include "mysql_result_info.hpp"

#include "tools/date.hpp"
#include "tools/time.hpp"
#include "tools/varchar.hpp"

#include "object/serializable.hpp"
#include "object/object_ptr.hpp"
#include "object/object_exception.hpp"
#include "object/prototype_tree.hpp"

#include <cstring>

namespace oos {

namespace mysql {

mysql_prepared_result::mysql_prepared_result(MYSQL_STMT *s, int rs, std::shared_ptr<oos::object_base_producer> producer)
  : detail::result_impl(producer)
  , affected_rows_((size_type)mysql_stmt_affected_rows(s))
  , rows((size_type)mysql_stmt_num_rows(s))
  , fields_(mysql_stmt_field_count(s))
  , stmt(s)
  , result_size(rs)
  , bind_(new MYSQL_BIND[rs])
  , info_(new mysql_result_info[rs])
{
    memset(bind_, 0, rs * sizeof(MYSQL_BIND));
    memset(info_, 0, rs * sizeof(mysql_result_info));
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
  result_index = 0;
  // prepare result array
  mysql_column_binder binder;
  binder.bind(o, info_, stmt, bind_);

  // fetch data
  int ret = mysql_stmt_fetch(stmt);
  if (ret == MYSQL_NO_DATA) {
    return false;
  } else if (ret == 1) {
    throw_stmt_error(ret, stmt, "mysql", "");
  } else {
//  if (ret == MYSQL_DATA_TRUNCATED) {
    // load data from database
    o->deserialize(*this);
//  }
  }

  return true;

//  return rows_-- > 0;
}

mysql_prepared_result::size_type mysql_prepared_result::affected_rows() const
{
  long ar = mysql_stmt_affected_rows(stmt);
  return ar;
  //return affected_rows_;
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

void mysql_prepared_result::read(const char */*id*/, char &/*x*/)
{
 ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, short &/*x*/)
{
 ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, int &/*x*/)
{
 ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, long &/*x*/)
{
 ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, unsigned char &/*x*/)
{
 ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, unsigned short &/*x*/)
{
 ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, unsigned int &/*x*/)
{
 ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, unsigned long &/*x*/)
{
 ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, bool &/*x*/)
{
 ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, float &/*x*/)
{
 ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, double &/*x*/)
{
 ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, char */*x*/, int /*s*/)
{
 ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, oos::date &x)
{
  if (info_[result_index].length > 0) {
    MYSQL_TIME *mtt = (MYSQL_TIME*)info_[result_index].buffer;
    x.set(mtt->day, mtt->month, mtt->year);
  }
  ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, oos::time &x)
{
  if (info_[result_index].length > 0) {
#if MYSQL_VERSION_ID < 50604
    // before mysql version 5.6.4 datetime
    // doesn't support fractional seconds
    // so we use a datetime string here
    char *data = (char*)bind_[result_index].buffer;
    unsigned long len = info_[result_index].length;
    std::string str(data,len);
    x.parse(str, "%F %T.%f");
#else
    MYSQL_TIME *mtt = (MYSQL_TIME*)info_[result_index].buffer;
    x.set(mtt->year, mtt->month, mtt->day, mtt->hour, mtt->minute, mtt->second, mtt->second_part / 1000);
#endif
  }
  ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, std::string &x)
{
  if (info_[result_index].length > 0) {
    bind_[result_index].buffer = new char[info_[result_index].length];
    bind_[result_index].buffer_length = info_[result_index].length;
    if (mysql_stmt_fetch_column(stmt, &bind_[result_index], result_index, 0) != 0) {
      // an error occured
    } else {
      char *data = (char*)bind_[result_index].buffer;
      unsigned long len = bind_[result_index].buffer_length;
      x.assign(data, len);
    }
    delete [] (char*)bind_[result_index].buffer;
    bind_[result_index].buffer = 0;
  }
  ++result_index;
}

void mysql_prepared_result::read(const char */*id*/, varchar_base &x)
{
  char *data = (char*)bind_[result_index].buffer;
//  unsigned long len = bind_[result_index].buffer_length;
  unsigned long len = info_[result_index].length;
  x.assign(data, len);
  ++result_index;
}

void mysql_prepared_result::read(const char *id, object_base_ptr &x)
{
    read_foreign_object(id, x);
}

void mysql_prepared_result::read(const char */*id*/, object_container &/*x*/) {}

void mysql_prepared_result::read(const char *id, basic_identifier &x)
{
  x.deserialize(id, *this);
}

}

}
