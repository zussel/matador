#ifndef MYSQL_COLUMN_FETCHER_HPP
#define MYSQL_COLUMN_FETCHER_HPP

#include "object/object_atomizer.hpp"

#include "mysql_prepared_result.hpp"

#ifdef WIN32
#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

namespace oos {

namespace mysql {

class mysql_column_fetcher : public generic_object_reader<mysql_column_fetcher>
{
public:
  mysql_column_fetcher(MYSQL_STMT *stmt, MYSQL_BIND *bind, mysql_prepared_result::result_info *info);
  virtual ~mysql_column_fetcher();
  
  void fetch(object *o);

  template < class T >
  void read_value(const char *, T &)
  {
    ++column_index_;
  }
  
  void read_value(const char *id, std::string &x);
  void read_value(const char *id, varchar_base &x);
  void read_value(const char *id, object_base_ptr &x);
  void read_value(const char *id, char *x, int s);
  
private:
  int column_index_;
  MYSQL_STMT *stmt_;
  MYSQL_BIND *bind_;
  mysql_prepared_result::result_info *info_;
};

}

}

#endif /* MYSQL_COLUMN_FETCHER_HPP */
