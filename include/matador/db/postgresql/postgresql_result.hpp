//
// Created by sascha on 29.05.19.
//

#ifndef MATADOR_POSTGRESQL_RESULT_HPP
#define MATADOR_POSTGRESQL_RESULT_HPP

#ifdef _MSC_VER
#ifdef matador_postgresql_EXPORTS
    #define MATADOR_POSTGRESQL_API __declspec(dllexport)
  #else
    #define MATADOR_POSTGRESQL_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
#define MATADOR_POSTGRESQL_API
#endif

#include <libpq-fe.h>

#include "matador/sql/result_impl.hpp"

namespace matador {

class row;
class serializer;

namespace postgresql {

class MATADOR_POSTGRESQL_API postgresql_result : public detail::result_impl
{
public:
  postgresql_result(const postgresql_result&) = delete;
  postgresql_result& operator=(const postgresql_result&) = delete;

public:
  typedef detail::result_impl::size_type size_type;

public:
  explicit postgresql_result(PGresult *res);
  ~postgresql_result() override;

  const char* column(size_type c) const override;
  bool fetch() override;

  size_type affected_rows() const override;
  size_type result_rows() const override;
  size_type fields() const override;

  int transform_index(int index) const override;

  PGresult* result_handle();

protected:
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
  void serialize(const char *id, varchar_base &x) override;
  void serialize(const char *id, std::string &x) override;
  void serialize(const char *id, matador::date &x) override;
  void serialize(const char *id, matador::time &x) override;
  void serialize(const char *id, matador::basic_identifier &x) override;
  void serialize(const char *id, matador::identifiable_holder &x, cascade_type) override;

protected:
  bool prepare_fetch() override;
  bool finalize_fetch() override;

private:
  size_type affected_rows_;
  size_type rows_;
  size_type fields_;

  size_type column_ = 0;
  size_type pos_ = 0;
  
  PGresult *res_ = nullptr;
};

}
}
#endif //MATADOR_POSTGRESQL_RESULT_HPP
