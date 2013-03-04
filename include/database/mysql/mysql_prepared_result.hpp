#ifndef MYSQL_PREPARED_RESULT_HPP
#define MYSQL_PREPARED_RESULT_HPP

#include "object/object_atomizable.hpp"
#include "object/object_atomizer.hpp"
#include "database/result.hpp"

#include <mysql/mysql.h>

#include <vector>
#include <iostream>

namespace oos {

namespace mysql {

class mysql_prepared_result : public result, public object_reader
{
private:
  mysql_prepared_result(const mysql_prepared_result&);
  mysql_prepared_result& operator=(const mysql_prepared_result&);

public:
  typedef unsigned long size_type;

public:
  mysql_prepared_result(MYSQL_STMT *s, MYSQL_BIND *b, int rs);
  ~mysql_prepared_result();
  
  void get(object_atomizable *o);
  const char* column(size_type c) const;
  bool fetch();
  size_type affected_rows() const;
  size_type result_rows() const;
  size_type fields() const;

  friend std::ostream& operator<<(std::ostream &out, const mysql_prepared_result &res);

protected:
  virtual void read(const char *id, long &x);
  virtual void read(const char *id, double &x);
  virtual void read(const char *id, std::string &x);

private:
  template < class T >
  void get_column_value(MYSQL_BIND &bind, enum_field_types , T &value)
  {
    value = *static_cast<T*>(bind.buffer);
  }

  void get_column_value(MYSQL_BIND &bind, enum_field_types type, std::string &value);

private:
  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  MYSQL_ROW row;
  MYSQL_RES *res;
  MYSQL_STMT *stmt;
  int result_index;
  int result_size;
  MYSQL_BIND *bind_;
};

std::ostream& operator<<(std::ostream &out, const mysql_prepared_result &res);

}

}

#endif /* MYSQL_PREPARED_RESULT_HPP */
