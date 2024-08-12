#ifndef QUERY_POSTGRES_PARAMETER_BINDER_H
#define QUERY_POSTGRES_PARAMETER_BINDER_H

#include "matador/object/attribute_binder.hpp"

#include <vector>

namespace matador::backends::postgres {

class postgres_parameter_binder final : public object::attribute_binder
{
public:
  explicit postgres_parameter_binder(size_t size);

  void bind(size_t pos, char i) override;
  void bind(size_t pos, short i) override;
  void bind(size_t pos, int i) override;
  void bind(size_t pos, long i) override;
  void bind(size_t pos, long long int i) override;
  void bind(size_t pos, unsigned char i) override;
  void bind(size_t pos, unsigned short i) override;
  void bind(size_t pos, unsigned int i) override;
  void bind(size_t pos, unsigned long i) override;
  void bind(size_t pos, unsigned long long int i) override;
  void bind(size_t pos, bool b) override;
  void bind(size_t pos, float d) override;
  void bind(size_t pos, double d) override;
  void bind( size_t pos, const time& time ) override;
  void bind( size_t pos, const date& date ) override;
  void bind(size_t pos, const char *str) override;
  void bind(size_t pos, const char *str, size_t size) override;
  void bind(size_t pos, const std::string &str) override;
  void bind(size_t pos, const std::string &x, size_t size) override;
  void bind(size_t pos, const utils::blob &blob) override;

  [[nodiscard]] const std::vector<const char*>& params() const;

private:
  std::vector<std::string> strings_;
  std::vector<const char*> params_;
};

}

#endif //QUERY_POSTGRES_PARAMETER_BINDER_H
