#ifndef QUERY_SQLITE_PARAMETER_BINDER_H
#define QUERY_SQLITE_PARAMETER_BINDER_H

#include "matador/object/attribute_binder.hpp"

#include <sql.h>

#include <memory>
#include <optional>
#include <vector>
#include <unordered_map>

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

public:
  struct bounded_value
  {
    explicit bounded_value(const SQLLEN l = 0)
    : len(l) {}

    SQLLEN len{};
    SQLLEN result_len = 0;
    std::unique_ptr<char[]> data{};
  };

  std::optional<std::optional<std::reference_wrapper<bounded_value>>> get_data_to_put(PTR ptr);

private:
  SQLHANDLE stmt_{};
  bool bind_null_ = false;
  std::vector<bounded_value> host_data_;
  std::unordered_map<PTR, std::reference_wrapper<bounded_value>> data_to_put_map_;
  std::vector<std::shared_ptr<std::string> > host_strings_;
};

}

#endif //QUERY_SQLITE_PARAMETER_BINDER_H
