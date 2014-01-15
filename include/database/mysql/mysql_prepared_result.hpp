#ifndef MYSQL_PREPARED_RESULT_HPP
#define MYSQL_PREPARED_RESULT_HPP

#include "database/result.hpp"

#ifdef WIN32
#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <vector>

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
  typedef result::size_type size_type;

public:
  mysql_prepared_result(MYSQL_STMT *s, int rs);
  ~mysql_prepared_result();
  
  const char* column(size_type c) const;
  bool fetch();
  
  bool fetch(object *o);
  
  size_type affected_rows() const;
  size_type result_rows() const;
  size_type fields() const;

  virtual int transform_index(int index) const;

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

public:
  struct result_info {
    unsigned long length;
    my_bool is_null;
    my_bool error;
    char *buffer;
    unsigned long buffer_length;
  };

private:
  template < class T >
  void prepare_bind_column(int index, enum_field_types type, T &value)
  {
    bind_[index].buffer_type = type /*MYSQL_TYPE_STRING*/;
    bind_[index].buffer= (char *)&value;
    bind_[index].buffer_length = sizeof(T);
    bind_[index].is_null = &info_[index].is_null;
    bind_[index].length = &info_[index].length;
    bind_[index].error = &info_[index].error;
  }

  void prepare_bind_column(int index, enum_field_types type, std::string &value);
  void prepare_bind_column(int index, enum_field_types type, char *x, int s);
  void prepare_bind_column(int index, enum_field_types type, varchar_base &value);
  void prepare_bind_column(int index, enum_field_types type, object_base_ptr &value);

private:
  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  MYSQL_ROW row;
  MYSQL_RES *res;
  MYSQL_STMT *stmt;
  int result_size;
  MYSQL_BIND *bind_;
  result_info *info_;
};

std::ostream& operator<<(std::ostream &out, const mysql_prepared_result &res);

}

}

#endif /* MYSQL_PREPARED_RESULT_HPP */
