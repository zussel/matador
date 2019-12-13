//
// Created by sascha on 08.09.19.
//

#ifndef MATADOR_POSTGRESQL_PARAMETER_BINDER_HPP
#define MATADOR_POSTGRESQL_PARAMETER_BINDER_HPP

#include <vector>
#include "matador/sql/parameter_binder.hpp"

namespace matador {

namespace postgresql {

class postgresql_parameter_binder : public detail::parameter_binder_impl
{
public:
  explicit postgresql_parameter_binder(size_t bind_var_size);
  void reset() override;

  void initialize_index(size_t index) override;
  size_t next_index() override;
  size_t current_index() const override;

  void bind(char i, size_t size) override;
  void bind(short i, size_t size) override;
  void bind(int i, size_t size) override;
  void bind(long i, size_t size) override;
  void bind(long long i, size_t size) override;
  void bind(unsigned char i, size_t size) override;
  void bind(unsigned short i, size_t size) override;
  void bind(unsigned int i, size_t size) override;
  void bind(unsigned long i, size_t size) override;
  void bind(unsigned long long i, size_t size) override;
  void bind(bool b, size_t size) override;
  void bind(float d, size_t size) override;
  void bind(double d, size_t size) override;
  void bind(const char *string, size_t size, size_t size1) override;
  void bind(const std::string &string, size_t size) override;
  void bind(const std::string &x, size_t s, size_t size) override;
  void bind(const matador::time &time, size_t size) override;
  void bind(const matador::date &date, size_t size) override;

  const std::vector<const char*>& params() const;

private:
  std::vector<std::string> strings_;
  std::vector<const char*> params_;

  size_t index_ = 0;
};

}
}
#endif //MATADOR_POSTGRESQL_PARAMETER_BINDER_HPP
