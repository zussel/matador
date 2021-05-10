//
// Created by sascha on 30.08.19.
//

#ifndef MATADOR_MSSQL_PARAMETER_BINDER_HPP
#define MATADOR_MSSQL_PARAMETER_BINDER_HPP

#include "matador/utils/time.hpp"
#include "matador/utils/basic_identifier.hpp"
#include "matador/sql/parameter_binder.hpp"

#include <sqltypes.h>

#include <vector>
#include <unordered_map>

namespace matador {

namespace mssql {

class mssql_parameter_binder : public detail::parameter_binder_impl
{
public:
  explicit mssql_parameter_binder(SQLHANDLE stmt);

  void reset() override;

  void initialize_index(size_t index) override;
  size_t next_index() override;
  size_t current_index() const override;

  void bind(char i, size_t) override;
  void bind(short i, size_t) override;
  void bind(int i, size_t) override;
  void bind(long i, size_t) override;
  void bind(long long i, size_t) override;
  void bind(unsigned char i, size_t) override;
  void bind(unsigned short i, size_t) override;
  void bind(unsigned int i, size_t) override;
  void bind(unsigned long i, size_t) override;
  void bind(unsigned long long i, size_t) override;
  void bind(bool b, size_t) override;
  void bind(float d, size_t) override;
  void bind(double d, size_t) override;
  void bind(const char *string, size_t size, size_t) override;
  void bind(const std::string &string, size_t) override;
  void bind(const std::string &x, size_t s, size_t) override;
  void bind(const matador::time &time, size_t) override;
  void bind(const matador::date &date, size_t) override;

  struct value_t
  {
    explicit value_t(SQLLEN l = 0) : len(l), data(nullptr)
    {}

    ~value_t()
    { delete[] data; }

    SQLLEN len;
    SQLLEN result_len = 0;
    char *data;
  };

  const std::unordered_map<PTR, value_t *>& data_to_put_map() const;

private:
  bool bind_null_ = false;

  std::vector<value_t *> host_data_;
  std::unordered_map<PTR, value_t *> data_to_put_map_;

  SQLHANDLE stmt_ = nullptr;

  size_t index_ = 1;
};

}

}
#endif //MATADOR_MSSQL_PARAMETER_BINDER_HPP
