#include "ColorEnumTraits.hpp"

#include "matador/object/attribute_writer.hpp"
#include "matador/object/attribute_reader.hpp"

namespace matador::object {

void data_type_traits<test::Color, void>::read_value(attribute_reader &reader, const char *id, size_t index,
                                                     test::Color &value)
{
  std::string enum_string;
  reader.read_value(id, index, enum_string, 64);
  if (const auto enum_opt = color_enum.to_enum(enum_string)) {
    value = enum_opt.value();
  }
}

void data_type_traits<test::Color, void>::bind_value(attribute_writer &binder, const size_t index, const test::Color &value)
{
  binder.bind(index, color_enum.to_string(value));
}

}
