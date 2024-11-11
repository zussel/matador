#ifndef ATTRIBUTE_STRING_WRITER_HPP
#define ATTRIBUTE_STRING_WRITER_HPP

#include "matador/utils/attribute_writer.hpp"

#include <optional>

namespace matador::sql {

class connection_impl;
class dialect;

class attribute_string_writer : public utils::attribute_writer
{
public:
  attribute_string_writer(const dialect &d, std::optional<std::reference_wrapper<const connection_impl>> conn);

  template<typename Type>
  [[nodiscard]] std::string to_string(const Type &value)
  {
      result_.clear();
      write_value(0, value);

      return result_;
  }
  [[nodiscard]] const sql::dialect& dialect() const;

  void write_value(size_t pos, const char& x) override;
  void write_value(size_t pos, const short& x) override;
  void write_value(size_t pos, const int& x) override;
  void write_value(size_t pos, const long& x) override;
  void write_value(size_t pos, const long long& x) override;
  void write_value(size_t pos, const unsigned char& x) override;
  void write_value(size_t pos, const unsigned short& x) override;
  void write_value(size_t pos, const unsigned& x) override;
  void write_value(size_t pos, const unsigned long& x) override;
  void write_value(size_t pos, const unsigned long long& x) override;
  void write_value(size_t pos, const bool& x) override;
  void write_value(size_t pos, const float& x) override;
  void write_value(size_t pos, const double& x) override;
  void write_value(size_t pos, const time& x) override;
  void write_value(size_t pos, const date& x) override;
  void write_value(size_t pos, const char* x) override;
  void write_value(size_t pos, const char* x, size_t size) override;
  void write_value(size_t pos, const std::string& x) override;
  void write_value(size_t pos, const std::string& x, size_t size) override;
  void write_value(size_t pos, const utils::blob& x) override;
  void write_value(size_t pos, const utils::value& x, size_t size) override;

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
