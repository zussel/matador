#ifndef MYSQL_PREPARED_RESULT_HPP
#define MYSQL_PREPARED_RESULT_HPP

#include "database/result.hpp"

#ifdef WIN32
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <vector>
#include <iostream>

namespace oos {

class object_base_ptr;
class varchar_base;

namespace mysql {

class mysql_prepared_result : public result
{
private:
  mysql_prepared_result(const mysql_prepared_result&);
  mysql_prepared_result& operator=(const mysql_prepared_result&);

public:
  typedef unsigned long size_type;

public:
  mysql_prepared_result(MYSQL_STMT *s, MYSQL_BIND *b, int rs);
  ~mysql_prepared_result();
  
  const char* column(size_type c) const;
  bool fetch();
  size_type affected_rows() const;
  size_type result_rows() const;
  size_type fields() const;

  friend std::ostream& operator<<(std::ostream &out, const mysql_prepared_result &res);

protected:
  virtual void read(const char *id, char &x);
  virtual void read(const char *id, short &x);
  virtual void read(const char *id, int &x);
  virtual void read(const char *id, long &x);
  virtual void read(const char *id, unsigned char &x);
  virtual void read(const char *id, unsigned short &x);
  virtual void read(const char *id, unsigned int &x);
  virtual void read(const char *id, unsigned long &x);
  virtual void read(const char *id, bool &x);
  virtual void read(const char *id, float &x);
  virtual void read(const char *id, double &x);
  virtual void read(const char *id, char *x, int s);
  virtual void read(const char *id, varchar_base &x);
  virtual void read(const char *id, std::string &x);
  virtual void read(const char *id, object_base_ptr &x);
  virtual void read(const char *id, object_container &x);

private:
  template < class T >
  void get_column_value(MYSQL_BIND &bind, enum_field_types , T &value)
  {
    value = *static_cast<T*>(bind.buffer);
  }

  void get_column_value(MYSQL_BIND &bind, enum_field_types type, char *x, int s);
  void get_column_value(MYSQL_BIND &bind, enum_field_types type, std::string &value);
  void get_column_value(MYSQL_BIND &bind, enum_field_types type, varchar_base &value);
  void get_column_value(MYSQL_BIND &bind, enum_field_types type, object_base_ptr &value);

private:
  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  MYSQL_ROW row;
  MYSQL_RES *res;
  MYSQL_STMT *stmt;
  int result_size;
  MYSQL_BIND *bind_;
};

std::ostream& operator<<(std::ostream &out, const mysql_prepared_result &res);

}

}

#endif /* MYSQL_PREPARED_RESULT_HPP */
