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
  void read_value(int index, int row, char &value) override;
  void read_value(int index, int row, short &value) override;
  void read_value(int index, int row, int &value) override;
  void read_value(int index, int row, long &value) override;
  void read_value(int index, int row, long long &value) override;
  void read_value(int index, int row, unsigned char &value) override;
  void read_value(int index, int row, unsigned short &value) override;
  void read_value(int index, int row, unsigned int &value) override;
  void read_value(int index, int row, unsigned long &value) override;
  void read_value(int index, int row, unsigned long long &value) override;
  void read_value(int index, int row, bool &value) override;
  void read_value(int index, int row, float &value) override;
  void read_value(int index, int row, double &value) override;
  void read_value(int index, int row, matador::time &value) override;
  void read_value(int index, int row, matador::date &value) override;
  void read_value(int index, int row, char *value, size_t s) override;
  void read_value(int index, int row, std::string &value) override;
  void read_value(int index, int row, std::string &value, size_t s) override;

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
