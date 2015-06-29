#include "mysql_column_fetcher.hpp"

#include "tools/date.hpp"
#include "tools/time.hpp"
#include "tools/varchar.hpp"

#include "object/object_ptr.hpp"
#include "object/serializable.hpp"

namespace oos {

namespace mysql {

mysql_column_fetcher::mysql_column_fetcher(MYSQL_STMT *stmt, MYSQL_BIND *bind, mysql_prepared_result::result_info *info)
  : generic_object_reader<mysql_column_fetcher>(this)
  , column_index_(0)
  , stmt_(stmt)
  , bind_(bind)
  , info_(info)
{}

mysql_column_fetcher::~mysql_column_fetcher()
{}

void mysql_column_fetcher::fetch(serializable *o)
{
  column_index_ = 0;
  o->deserialize(*this);
}

void mysql_column_fetcher::read_value(const char *, oos::date &x)
{
  if (info_[column_index_].length > 0) {
    MYSQL_TIME *mtt = (MYSQL_TIME*)info_[column_index_].buffer;
    x.set(mtt->day, mtt->month, mtt->year);
  }
  ++column_index_;
}

void mysql_column_fetcher::read_value(const char *, oos::time &x)
{
  if (info_[column_index_].length > 0) {
    MYSQL_TIME *mtt = (MYSQL_TIME*)info_[column_index_].buffer;
    x.set(mtt->year, mtt->month, mtt->day, mtt->hour, mtt->minute, mtt->second, mtt->second_part / 1000);
  }
  ++column_index_;
}

void mysql_column_fetcher::read_value(const char *, std::string &x)
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

void mysql_column_fetcher::read_value(const char *, varchar_base &x)
{
  char *data = (char*)bind_[column_index_].buffer;
//  unsigned long len = bind_[column_index_].buffer_length;
  unsigned long len = info_[column_index_].length;
  x.assign(data, len);
  ++column_index_;
}

void mysql_column_fetcher::read_value(const char *, object_base_ptr &x)
{
  long id = *static_cast<long*>(bind_[column_index_].buffer);
  x.id(id);
}

void mysql_column_fetcher::read_value(const char *, char *, int )
{
  ++column_index_;
}

}

}
