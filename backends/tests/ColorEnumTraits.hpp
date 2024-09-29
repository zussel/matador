#ifndef MATADOR_COLOR_ENUM_TRAITS_HPP
#define MATADOR_COLOR_ENUM_TRAITS_HPP

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

template<>
struct matador::object::data_type_traits<matador::test::Color, void>
{
  static data_type type(const std::size_t size) { return data_type_traits<std::string>::type(size); }
  static void read_value(attribute_reader &reader, const char *id, size_t index, test::Color &value);
  static void bind_value(attribute_writer &binder, size_t index, const test::Color &value);
};

#endif //MATADOR_COLOR_ENUM_TRAITS_HPP
