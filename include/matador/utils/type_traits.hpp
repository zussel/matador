#ifndef BASIC_TYPE_TRAITS_HPP
#define BASIC_TYPE_TRAITS_HPP

#include "matador/utils/types.hpp"

#include <cstddef>

namespace matador::utils {

class value_reader;
class value_writer;

/**
 * @tparam Type The type of the traits
 * @brief Type traits for database types
 *
 * This class is used to determine and
 * provide the correct size information
 * for a data type
 */
template < class Type, class Enable = void >
struct data_type_traits {
  static basic_type type(std::size_t /*size*/) { return basic_type::Unknown; }
  static void read_value(value_reader &/*reader*/, const char *id, size_t index, nullptr_t &/*value*/, size_t /*size*/ = 0) {}
  static void bind_value(value_writer &/*binder*/, size_t index, nullptr_t &/*value*/, size_t /*size*/ = 0) {}
};

}
#endif //BASIC_TYPE_TRAITS_HPP
