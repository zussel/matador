#ifndef MYSQL_COLUMN_FETCHER_HPP
#define MYSQL_COLUMN_FETCHER_HPP

#include "object/object_atomizer.hpp"

#include "mysql_result_info.hpp"

#ifdef WIN32
#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <unordered_map>
#include <memory>

namespace oos {

class serializable;
class prototype_node;
class date;
class time;

namespace mysql {

class mysql_column_binder : public object_reader
{
public:
  mysql_column_binder();
  virtual ~mysql_column_binder();
  
  void bind(serializable *o, const prototype_node *node, std::unordered_map<std::string, std::shared_ptr<basic_identifier> > *pk_map,
            mysql_result_info *info, MYSQL_STMT *stmt, MYSQL_BIND *bind);

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
  virtual void read(const char *id, oos::date &x);
  virtual void read(const char *id, oos::time &x);
  virtual void read(const char *id, object_base_ptr &x);
  virtual void read(const char *id, object_container &x);
  virtual void read(const char *id, basic_identifier &x);
  
private:
  template < class T >
  void prepare_bind_column(int index, enum_field_types type, T &value)
  {
    bind_[index].buffer_type = type;
    bind_[index].buffer= (char *)&value;
    bind_[index].buffer_length = sizeof(T);
    bind_[index].is_null = &info_[index].is_null;
    bind_[index].length = &info_[index].length;
    bind_[index].error = &info_[index].error;
  }

  void prepare_bind_column(int index, enum_field_types type, oos::date &value);
  void prepare_bind_column(int index, enum_field_types type, oos::time &value);
  void prepare_bind_column(int index, enum_field_types type, std::string &value);
  void prepare_bind_column(int index, enum_field_types type, char *x, int s);
  void prepare_bind_column(int index, enum_field_types type, varchar_base &value);
  void prepare_bind_column(int index, enum_field_types type, object_base_ptr &value);

private:
  int column_index_ = 0;
  MYSQL_STMT *stmt_ = nullptr;
  MYSQL_BIND *bind_ = nullptr;
  mysql_result_info *info_ = nullptr;
  const prototype_node *node_;

  std::unordered_map<std::string, std::shared_ptr<basic_identifier> > *pk_map_;
};

}

}

#endif /* MYSQL_COLUMN_FETCHER_HPP */
