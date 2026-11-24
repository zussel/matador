#include "ColorEnumTraits.hpp"

#include "matador/query/mapping/value_reader.hpp"
#include "matador/query/mapping/value_writer.hpp"

namespace matador::query {
void data_type_traits<test::Color>::read_value(value_reader &reader, const char *id, const size_t index, test::Color &value) {
  std::string enum_string;
  reader.read_value(id, index, enum_string, 64);
  if (const auto enum_opt = color_enum.to_enum(enum_string)) {
    value = enum_opt.value();
  }
}

void data_type_traits<test::Color>::bind_value(value_writer &binder, const size_t index, const test::Color &value) {
  binder.write_value(index, color_enum.to_string(value).value_or("Unknown"));
}
}
