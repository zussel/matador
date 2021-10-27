#ifndef MYSQL_PREPARED_RESULT_HPP
#define MYSQL_PREPARED_RESULT_HPP

#include "matador/sql/result_impl.hpp"

#include "matador/db/mysql/mysql_result_info.hpp"
#include "matador/db/mysql/mysql_bool.hpp"

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <vector>
#include <unordered_map>
#include <string>
#include <cstring>
#include <memory>

namespace matador {

class object_base_ptr;
class basic_identifier;
class time;
class date;

namespace mysql {

class mysql_statement;

class mysql_prepared_result : public detail::result_impl
{
public:
  mysql_prepared_result(const mysql_prepared_result &) = delete;
  mysql_prepared_result &operator=(const mysql_prepared_result &) = delete;

public:
  typedef detail::result_impl::size_type size_type;

public:
  mysql_prepared_result(mysql_statement *owner, MYSQL_STMT *s, std::vector<MYSQL_BIND> &bind, std::vector<mysql_result_info> &info);
  ~mysql_prepared_result() override;

  const char* column(size_type c) const override;
  bool fetch() override;

  size_type affected_rows() const override;
  size_type result_rows() const override;
  size_type fields() const override;

  int transform_index(int index) const override;

  void serialize(const char *id, char &x) override;
  void serialize(const char *id, short &x) override;
  void serialize(const char *id, int &x) override;
  void serialize(const char *id, long &x) override;
  void serialize(const char *id, long long &x) override;
  void serialize(const char *id, unsigned char &x) override;
  void serialize(const char *id, unsigned short &x) override;
  void serialize(const char *id, unsigned int &x) override;
  void serialize(const char *id, unsigned long &x) override;
  void serialize(const char *id, unsigned long long &x) override;
  void serialize(const char *id, bool &x) override;
  void serialize(const char *id, float &x) override;
  void serialize(const char *id, double &x) override;
  void serialize(const char *id, char *x, size_t s) override;
  void serialize(const char *id, matador::date &x) override;
  void serialize(const char *id, matador::time &x) override;
  void serialize(const char *id, std::string &x) override;
  void serialize(const char *id, std::string &x, size_t s) override;
  void serialize(const char *id, basic_identifier &x) override;
  void serialize(const char *id, identifiable_holder &x, cascade_type) override;

  void free();
  
protected:
  bool needs_bind() override;
  bool finalize_bind() override;

  bool prepare_fetch() override;
  bool finalize_fetch() override;

private:

  template < class T >
  void prepare_bind_column(int index, enum_field_types type, T &value)
  {
    bind_[index].buffer= (char *)&value;
    bind_[index].buffer_length = sizeof(T);
    bind_[index].buffer_type = type;
    bind_[index].is_null = &info_[index].is_null;
    bind_[index].length = &info_[index].length;
    bind_[index].error = &info_[index].error;
  }

  void prepare_bind_column(int index, enum_field_types type, matador::date &value);
  void prepare_bind_column(int index, enum_field_types type, matador::time &value);
  void prepare_bind_column(int index, enum_field_types type, std::string &value);
  void prepare_bind_column(int index, enum_field_types type, std::string &value, size_t s);
  void prepare_bind_column(int index, enum_field_types type, char *x, size_t s);

  void on_truncated_data(int index, std::string &x);

private:
  int column_index_ = 0;
  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  mysql_statement *owner_ = nullptr;
  MYSQL_STMT *stmt = nullptr;
//  unsigned int result_size;

  std::vector<MYSQL_BIND> &bind_;
  std::vector<mysql_result_info> &info_;

  bool prepare_binding_ = true;

  typedef std::unordered_map<std::string, std::unique_ptr<basic_identifier> > t_foreign_key_map;
  t_foreign_key_map foreign_keys_;
};

}

}

#endif /* MYSQL_PREPARED_RESULT_HPP */
