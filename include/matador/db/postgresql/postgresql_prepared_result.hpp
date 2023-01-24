#ifndef MATADOR_POSTGRESQL_PREPARED_RESULT_HPP
#define MATADOR_POSTGRESQL_PREPARED_RESULT_HPP

#include "matador/sql/result_impl.hpp"

#include <libpq-fe.h>

namespace matador {
namespace postgresql {

class postgresql_statement;

class postgresql_prepared_result : public detail::result_impl
{
public:
  postgresql_prepared_result(const postgresql_prepared_result &) = delete;
  postgresql_prepared_result &operator=(const postgresql_prepared_result &) = delete;

public:
  typedef detail::result_impl::size_type size_type;

public:
  postgresql_prepared_result(postgresql_statement *owner, PGresult *s);
  ~postgresql_prepared_result() override;

  const char* column(size_type c) const override;
  bool fetch() override;

  size_type affected_rows() const override;
  size_type result_rows() const override;
  size_type fields() const override;

  int transform_index(int index) const override;

  void read_value(const char *id, int index, int row, char &x) override;
  void read_value(const char *id, int index, int row, short &x) override;
  void read_value(const char *id, int index, int row, int &x) override;
  void read_value(const char *id, int index, int row, long &x) override;
  void read_value(const char *id, int index, int row, long long &x) override;
  void read_value(const char *id, int index, int row, unsigned char &x) override;
  void read_value(const char *id, int index, int row, unsigned short &x) override;
  void read_value(const char *id, int index, int row, unsigned int &x) override;
  void read_value(const char *id, int index, int row, unsigned long &x) override;
  void read_value(const char *id, int index, int row, unsigned long long &x) override;
  void read_value(const char *id, int index, int row, bool &x) override;
  void read_value(const char *id, int index, int row, float &x) override;
  void read_value(const char *id, int index, int row, double &x) override;
  void read_value(const char *id, int index, int row, matador::date &x) override;
  void read_value(const char *id, int index, int row, matador::time &x) override;
  void read_value(const char *id, int index, int row, char *x, long size) override;
  void read_value(const char *id, int index, int row, std::string &x) override;
  void read_value(const char *id, int index, int row, std::string &x, long size) override;

protected:
  bool needs_bind() override;
  bool finalize_bind() override;

  bool prepare_fetch() override;
  bool finalize_fetch() override;

private:
  PGresult *res_ = nullptr;
  size_type row_ = 0;
  size_type column_ = 0;

  size_type affected_rows_ = 0;
  size_type rows_ = 0;
  size_type fields_ = 0;
};

}
}

#endif //MATADOR_POSTGRESQL_PREPARED_RESULT_HPP
