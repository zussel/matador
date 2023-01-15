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

  int transform_index(int index) const override;

protected:
  void on_attribute(const char *id, char &x) override;
  void on_attribute(const char *id, short &x) override;
  void on_attribute(const char *id, int &x) override;
  void on_attribute(const char *id, long &x) override;
  void on_attribute(const char *id, long long &x) override;
  void on_attribute(const char *id, unsigned char &x) override;
  void on_attribute(const char *id, unsigned short &x) override;
  void on_attribute(const char *id, unsigned int &x) override;
  void on_attribute(const char *id, unsigned long &x) override;
  void on_attribute(const char *id, unsigned long long &x) override;
  void on_attribute(const char *id, bool &x) override;
  void on_attribute(const char *id, float &x) override;
  void on_attribute(const char *id, double &x) override;
  void on_attribute(const char *id, char *x, size_t s) override;
  void on_attribute(const char *id, matador::date &x) override;
  void on_attribute(const char *id, matador::time &x) override;
  void on_attribute(const char *id, std::string &x) override;
  void on_attribute(const char *id, std::string &x, size_t s) override;
  void on_primary_key(const char *id, basic_identifier &x) override;
  void on_belongs_to(const char *id, identifiable_holder &x, cascade_type) override;
  void on_has_one(const char *id, identifiable_holder &x, cascade_type) override;

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
