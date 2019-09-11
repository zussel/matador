//
// Created by sascha on 06.09.19.
//

#ifndef MATADOR_MYSQL_PARAMETER_BINDER_HPP
#define MATADOR_MYSQL_PARAMETER_BINDER_HPP

#include "matador/sql/parameter_binder.hpp"

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <vector>

namespace matador {

namespace mysql {

class mysql_parameter_binder : public detail::parameter_binder_impl
{
public:

  void reset() override;

  void initialize_index(size_t index) override;
  size_t next_index() override;
  size_t current_index() const override;

  void bind(char i, size_t size) override;
  void bind(short i, size_t size) override;
  void bind(int i, size_t size) override;
  void bind(long i, size_t size) override;
  void bind(unsigned char i, size_t size) override;
  void bind(unsigned short i, size_t size) override;
  void bind(unsigned int i, size_t size) override;
  void bind(unsigned long i, size_t size) override;
  void bind(bool b, size_t size) override;
  void bind(float d, size_t size) override;
  void bind(double d, size_t size) override;
  void bind(const char *string, size_t size, size_t size1) override;
  void bind(const std::string &string, size_t size) override;
  void bind(const std::string &x, size_t s, size_t size) override;
  void bind(const matador::time &time, size_t size) override;
  void bind(const matador::date &date, size_t size) override;

private:
  std::vector<MYSQL_BIND> bind_;
  std::vector<my_bool> is_null_vector;

  size_t index_ = 0;
};

}

}
#endif //MATADOR_MYSQL_PARAMETER_BINDER_HPP
