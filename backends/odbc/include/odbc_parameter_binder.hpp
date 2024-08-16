#ifndef QUERY_SQLITE_PARAMETER_BINDER_H
#define QUERY_SQLITE_PARAMETER_BINDER_H

#include "matador/object/attribute_binder.hpp"

#include <sql.h>

#include <memory>
#include <vector>

namespace matador::backends::odbc {

class odbc_parameter_binder final : public object::attribute_binder
{
public:
  explicit odbc_parameter_binder(SQLHANDLE stmt);

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
  void bind(size_t pos, const time &time) override;
  void bind(size_t pos, const date &date) override;
  void bind(size_t pos, const char *string) override;
  void bind(size_t pos, const char *str, size_t size) override;
  void bind(size_t pos, const std::string &str) override;
  void bind(size_t pos, const std::string &str, size_t size) override;
  void bind(size_t pos, const utils::blob &blob) override;
private:
  SQLHANDLE stmt_{};
  std::vector<std::shared_ptr<std::string> > host_strings_;
};
}

#endif //QUERY_SQLITE_PARAMETER_BINDER_H
