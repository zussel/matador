#include "ColorEnumTraits.hpp"

#include "matador/object/attribute_binder.hpp"
#include "matador/object/attribute_reader.hpp"
//#include "matador/sql/result_parameter_binder.hpp"

namespace matador::object {

void data_type_traits<test::Color, void>::read_value(attribute_reader &reader, const char *id, size_t index,
                                                     test::Color &value)
{
  std::string enum_string;
  reader.read_value(id, index, enum_string, 64);
  auto enum_opt = color_enum.to_enum(enum_string);
  if (enum_opt) {
    value = enum_opt.value();
  }
}

void data_type_traits<test::Color, void>::bind_value(attribute_binder &binder, size_t index, test::Color &value)
{
  binder.bind(index, color_enum.to_string(value));
}

}
