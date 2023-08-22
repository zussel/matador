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

  size_type reset_column_index() const override;

  void read_value(const char *id, size_type index, char &x) override;
  void read_value(const char *id, size_type index, short &x) override;
  void read_value(const char *id, size_type index, int &x) override;
  void read_value(const char *id, size_type index, long &x) override;
  void read_value(const char *id, size_type index, long long &x) override;
  void read_value(const char *id, size_type index, unsigned char &x) override;
  void read_value(const char *id, size_type index, unsigned short &x) override;
  void read_value(const char *id, size_type index, unsigned int &x) override;
  void read_value(const char *id, size_type index, unsigned long &x) override;
  void read_value(const char *id, size_type index, unsigned long long &x) override;
  void read_value(const char *id, size_type index, bool &x) override;
  void read_value(const char *id, size_type index, float &x) override;
  void read_value(const char *id, size_type index, double &x) override;
  void read_value(const char *id, size_type index, matador::date &x) override;
  void read_value(const char *id, size_type index, matador::time &x) override;
  void read_value(const char *id, size_type index, char *x, size_t size) override;
  void read_value(const char *id, size_type index, std::string &x) override;
  void read_value(const char *id, size_type index, std::string &x, size_t size) override;

protected:
  bool needs_bind() override;
  bool finalize_bind() override;

  bool prepare_fetch() override;
  bool finalize_fetch() override;

private:
  PGresult *res_ = nullptr;

  size_type affected_rows_ = 0;
  size_type rows_ = 0;
  size_type fields_ = 0;

  size_type row_index_ = 0;
};

}
}

#endif //MATADOR_POSTGRESQL_PREPARED_RESULT_HPP
