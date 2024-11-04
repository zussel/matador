#ifndef MATADOR_VALUE_TYPE_TRAITS_HPP
#define MATADOR_VALUE_TYPE_TRAITS_HPP

#include "matador/utils/data_type_traits.hpp"
#include "matador/utils/value.hpp"

namespace matador::utils {

template <> struct data_type_traits<value, void>
{
  static data_type type(std::size_t /*size*/) { return data_type::type_time; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, value &val);
  static void bind_value(attribute_writer &binder, size_t index, value &val, size_t size);
};

}

#endif //MATADOR_VALUE_TYPE_TRAITS_HPP
