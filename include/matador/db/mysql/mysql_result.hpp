#ifndef MYSQL_RESULT_HPP
#define MYSQL_RESULT_HPP

#include "matador/sql/result_impl.hpp"

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <vector>

namespace matador {

class row;
class serializer;

namespace mysql {

class mysql_result : public detail::result_impl
{
public:
  mysql_result(const mysql_result&) = delete;
  mysql_result& operator=(const mysql_result&) = delete;

public:
  typedef detail::result_impl::size_type size_type;

public:
  explicit mysql_result(MYSQL *c);
  ~mysql_result() override;

  const char* column(size_type c) const override;
  bool fetch() override;
  
  size_type affected_rows() const override;
  size_type result_rows() const override;
  size_type fields() const override;

  size_type reset_column_index() const override;

  void close() override;

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
  bool prepare_fetch() override;
  bool finalize_fetch() override;

private:
  size_type affected_rows_ = 0;
  size_type rows_ = 0;
  size_type fields_ = 0;
  MYSQL_ROW row_;
  MYSQL_RES *res_ = nullptr;
};

}

}

#endif /* MYSQL_RESULT_HPP */
