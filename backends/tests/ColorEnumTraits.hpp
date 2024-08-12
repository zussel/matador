#ifndef MATADOR_COLORENUMTRAITS_HPP
#define MATADOR_COLORENUMTRAITS_HPP

#include "matador/object/data_type_traits.hpp"

#include "matador/utils/enum_mapper.hpp"

#include "models/location.hpp"

static const matador::utils::enum_mapper<matador::test::Color> color_enum({
                                                           {matador::test::Color::Green,  "green"},
                                                           {matador::test::Color::Red,    "red"},
                                                           {matador::test::Color::Blue,   "blue"},
                                                           {matador::test::Color::Yellow, "yellow"},
                                                           {matador::test::Color::Black,  "black"},
                                                           {matador::test::Color::White,  "white"},
                                                           {matador::test::Color::Brown,  "brown"}
                                                           });

namespace matador::object {

template<>
struct data_type_traits<test::Color, void>
{
  inline static data_type type(std::size_t size) { return data_type_traits<std::string>::type(size); }
  static void read_value(attribute_reader &reader, const char *id, size_t index, test::Color &value);
  static void bind_value(attribute_binder &binder, size_t index, test::Color &value);
//  static void bind_result_value(result_parameter_binder &binder, size_t index, test::Color &value);
//  static any_type create_value(test::Color &value) { return color_enum.to_string(value); }
};

}

#endif //MATADOR_COLORENUMTRAITS_HPP
