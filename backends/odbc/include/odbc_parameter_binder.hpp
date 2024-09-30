#ifndef QUERY_SQLITE_PARAMETER_BINDER_H
#define QUERY_SQLITE_PARAMETER_BINDER_H

#include "matador/object/attribute_writer.hpp"

#include <sql.h>

#include <memory>
#include <optional>
#include <vector>
#include <unordered_map>

namespace matador::backends::odbc {

class odbc_parameter_binder final : public object::attribute_writer
{
public:
  explicit odbc_parameter_binder(SQLHANDLE stmt);

  void write_value(size_t pos, const char &x) override;
  void write_value(size_t pos, const short &x) override;
  void write_value(size_t pos, const int &x) override;
  void write_value(size_t pos, const long &x) override;
  void write_value(size_t pos, const long long int &x) override;
  void write_value(size_t pos, const unsigned char &x) override;
  void write_value(size_t pos, const unsigned short &x) override;
  void write_value(size_t pos, const unsigned int &x) override;
  void write_value(size_t pos, const unsigned long &x) override;
  void write_value(size_t pos, const unsigned long long int &x) override;
  void write_value(size_t pos, const bool &x) override;
  void write_value(size_t pos, const float &x) override;
  void write_value(size_t pos, const double &x) override;
  void write_value(size_t pos, const time &x ) override;
  void write_value(size_t pos, const date &x ) override;
  void write_value(size_t pos, const char *x) override;
  void write_value(size_t pos, const char *x, size_t size) override;
  void write_value(size_t pos, const std::string &x) override;
  void write_value(size_t pos, const std::string &x, size_t size) override;
  void write_value(size_t pos, const utils::blob &x) override;

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
