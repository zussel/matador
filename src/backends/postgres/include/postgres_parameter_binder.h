#ifndef QUERY_POSTGRES_PARAMETER_BINDER_H
#define QUERY_POSTGRES_PARAMETER_BINDER_H

#include "matador/query/mapping/value_writer.hpp"

#include <vector>

namespace matador::backends::postgres {

class postgres_parameter_binder final : public query::value_writer {
public:
  struct bind_data {
    explicit bind_data(size_t size);
    std::vector<std::string> strings;
    std::vector<std::vector<unsigned char>> bytes;
    std::vector<const char*> values;
    std::vector<int> lengths;
    std::vector<int> formats;
  };

  explicit postgres_parameter_binder(size_t size);

  void write_value(size_t pos, const uint8_t &x) override;
  void write_value(size_t pos, const uint16_t &x) override;
  void write_value(size_t pos, const uint32_t &x) override;
  void write_value(size_t pos, const uint64_t &x) override;
  void write_value(size_t pos, const int8_t &x) override;
  void write_value(size_t pos, const int16_t &x) override;
  void write_value(size_t pos, const int32_t &x) override;
  void write_value(size_t pos, const int64_t &x) override;
  void write_value(size_t pos, const bool &x) override;
  void write_value(size_t pos, const float &x) override;
  void write_value(size_t pos, const double &x) override;
  void write_value(size_t pos, const utils::date_type_t &x ) override;
  void write_value(size_t pos, const utils::time_type_t &x ) override;
  void write_value(size_t pos, const utils::timestamp_type_t &x ) override;
  void write_value(size_t pos, const char *x) override;
  void write_value(size_t pos, const char *x, size_t size) override;
  void write_value(size_t pos, const std::string &x) override;
  void write_value(size_t pos, const std::string &x, size_t size) override;
  void write_value(size_t pos, const utils::blob_type_t &x) override;
  void write_value(size_t pos, const query::column_value &x, size_t size) override;

  [[nodiscard]] const bind_data& params() const;

private:
  bind_data bind_data_;
//  std::vector<std::string> strings_;
//  std::vector<const char*> params_;
};

}

#endif //QUERY_POSTGRES_PARAMETER_BINDER_H
