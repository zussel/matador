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

  size_type reset_column_index() const override;

  void free();

protected:
  void read_value(const char *id, size_type index, char &value) override;
  void read_value(const char *id, size_type index, short &value) override;
  void read_value(const char *id, size_type index, int &value) override;
  void read_value(const char *id, size_type index, long &value) override;
  void read_value(const char *id, size_type index, long long &value) override;
  void read_value(const char *id, size_type index, unsigned char &value) override;
  void read_value(const char *id, size_type index, unsigned short &value) override;
  void read_value(const char *id, size_type index, unsigned int &value) override;
  void read_value(const char *id, size_type index, unsigned long &value) override;
  void read_value(const char *id, size_type index, unsigned long long &value) override;
  void read_value(const char *id, size_type index, bool &value) override;
  void read_value(const char *id, size_type index, float &value) override;
  void read_value(const char *id, size_type index, double &value) override;
  void read_value(const char *id, size_type index, matador::time &value) override;
  void read_value(const char *id, size_type index, matador::date &value) override;
  void read_value(const char *id, size_type index, char *value, size_t size) override;
  void read_value(const char *id, size_type index, std::string &value) override;
  void read_value(const char *id, size_type index, std::string &value, size_t size) override;

protected:
  bool needs_bind() override;
  bool finalize_bind() override;

  bool prepare_fetch() override;
  bool finalize_fetch() override;

  void read_foreign_object(const char *id, identifiable_holder &x) override;

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
  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  mysql_statement *owner_ = nullptr;
  MYSQL_STMT *stmt = nullptr;
//  unsigned int result_size;

  std::vector<MYSQL_BIND> &bind_;
  std::vector<mysql_result_info> &info_;

  bool prepare_binding_ = true;

  typedef std::unordered_map<std::string, identifier> t_foreign_key_map;
  t_foreign_key_map foreign_keys_;
};

}

}

#endif /* MYSQL_PREPARED_RESULT_HPP */
