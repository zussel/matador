//
// Created by sascha on 11.06.19.
//

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

  void serialize(const char *id, char &x) override;
  void serialize(const char *id, short &x) override;
  void serialize(const char *id, int &x) override;
  void serialize(const char *id, long &x) override;
  void serialize(const char *id, unsigned char &x) override;
  void serialize(const char *id, unsigned short &x) override;
  void serialize(const char *id, unsigned int &x) override;
  void serialize(const char *id, unsigned long &x) override;
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
