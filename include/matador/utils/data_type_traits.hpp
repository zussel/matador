#ifndef QUERY_DATA_TYPE_TRAITS_HPP
#define QUERY_DATA_TYPE_TRAITS_HPP

#include "matador/utils/data_types.hpp"

namespace matador::utils {

class attribute_reader;
class attribute_writer;

/**
 * @tparam T The type of the traits
 * @brief Type traits for database types
 *
 * This class is used to determine and
 * provide the correct size information
 * for a data type
 */
template < class Type, class Enable = void >
struct data_type_traits
{
   static data_type type(std::size_t /*size*/) { return data_type::type_unknown; }
   static void read_value(attribute_reader &, const char *, size_t, Type &/*value*/) {}
   static void bind_value(attribute_writer &, size_t, Type &/*value*/) {}
};

}
#endif //QUERY_DATA_TYPE_TRAITS_HPP
