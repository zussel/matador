#ifndef ATTRIBUTE_STRING_WRITER_HPP
#define ATTRIBUTE_STRING_WRITER_HPP

#include "matador/query/value_writer.hpp"

#include "matador/query/interface/connection_impl.hpp"

#include <optional>

namespace matador::query {
class dialect;

class attribute_string_writer final : public value_writer {
public:
  attribute_string_writer(const dialect &d, std::optional<std::reference_wrapper<const connection_impl>> conn);

  template<typename Type>
  [[nodiscard]] std::string to_string(const Type &value)
  {
      result_.clear();
      write_value(0, value);

      return result_;
  }
  [[nodiscard]] const class dialect& dialect() const;

  void write_value(size_t pos, const int8_t& x) override;
  void write_value(size_t pos, const int16_t& x) override;
  void write_value(size_t pos, const int32_t& x) override;
  void write_value(size_t pos, const int64_t& x) override;
  void write_value(size_t pos, const uint8_t& x) override;
  void write_value(size_t pos, const uint16_t& x) override;
  void write_value(size_t pos, const uint32_t& x) override;
  void write_value(size_t pos, const uint64_t& x) override;
  void write_value(size_t pos, const bool& x) override;
  void write_value(size_t pos, const float& x) override;
  void write_value(size_t pos, const double& x) override;
  void write_value(size_t pos, const utils::date_type_t& x) override;
  void write_value(size_t pos, const utils::time_type_t& x) override;
  void write_value(size_t pos, const utils::timestamp_type_t& x) override;
  void write_value(size_t pos, const char* x) override;
  void write_value(size_t pos, const char* x, size_t size) override;
  void write_value(size_t pos, const std::string& x) override;
  void write_value(size_t pos, const std::string& x, size_t size) override;
  void write_value(size_t pos, const utils::blob_type_t& x) override;
  void write_value(size_t pos, const column_value& x, size_t size) override;

private:
  std::string result_;
  const class dialect &dialect_;
  std::optional<std::reference_wrapper<const connection_impl>> conn_;
};

// "This is a binary Data string" as binary data:
// MySQL:    X'5468697320697320612062616E617279204461746120737472696E67'
// Postgres: E'\\x5468697320697320612062616E617279204461746120737472696E67'
// MSSQL:    0x5468697320697320612062616E617279204461746120737472696E67
// Sqlite:   X'5468697320697320612062616E617279204461746120737472696E67'

}
#endif //ATTRIBUTE_STRING_WRITER_HPP
