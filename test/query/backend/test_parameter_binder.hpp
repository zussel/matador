#ifndef TEST_PARAMETER_BINDER_HPP
#define TEST_PARAMETER_BINDER_HPP

#include "matador/query/value_writer.hpp"

namespace matador::test::orm {

class test_parameter_binder final : public query::value_writer {
public:
  void write_value(size_t pos, const int8_t &x) override;
  void write_value(size_t pos, const int16_t &x) override;
  void write_value(size_t pos, const int32_t &x) override;
  void write_value(size_t pos, const int64_t &x) override;
  void write_value(size_t pos, const uint8_t &x) override;
  void write_value(size_t pos, const uint16_t &x) override;
  void write_value(size_t pos, const uint32_t &x) override;
  void write_value(size_t pos, const uint64_t &x) override;
  void write_value(size_t pos, const bool &x) override;
  void write_value(size_t pos, const float &x) override;
  void write_value(size_t pos, const double &x) override;
  void write_value(size_t pos, const utils::date_type_t &x) override;
  void write_value(size_t pos, const utils::time_type_t &x) override;
  void write_value(size_t pos, const utils::timestamp_type_t &x) override;
  void write_value(size_t pos, const char *x) override;
  void write_value(size_t pos, const char *x, size_t size) override;
  void write_value(size_t pos, const std::string &x) override;
  void write_value(size_t pos, const std::string &x, size_t size) override;
  void write_value(size_t pos, const utils::blob_type_t &x) override;
  void write_value(size_t pos, const query::column_value &x, size_t size) override;
};

}

#endif //TEST_PARAMETER_BINDER_HPP
