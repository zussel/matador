#ifndef QUERY_DATA_TYPE_TRAITS_HPP
#define QUERY_DATA_TYPE_TRAITS_HPP

#include "matador/utils/data_types.hpp"
#include "matador/utils/types.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include <cstdint>
#include <string>

namespace matador::object {

class attribute_reader;
class attribute_binder;

/**
 * @tparam T The type of the traits
 * @brief Type traits for database types
 *
 * This class is used to determine and
 * provide the correct size information
 * for a data type
 */
template < class Type, class Enable = void >
struct data_type_traits;
// {
  // inline static data_type type(std::size_t /*size*/) { return data_type::type_unknown; }
  // static void read_value(attribute_reader &reader, const char *id, size_t index, Type &/*value*/);
  // static void bind_value(attribute_binder &binder, size_t index, Type &/*value*/);
//  static void bind_result_value(result_parameter_binder &binder, size_t index, Type &/*value*/);
//  inline static any_type create_value(Type &/*value*/) { return {}; }
// };

/// @cond MATADOR_DEV
template <> struct data_type_traits<nullptr_t, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_null; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, nullptr_t &/*value*/);
  static void bind_value(attribute_binder &binder, size_t index, nullptr_t &/*value*/);
};

template <> struct data_type_traits<char, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_char; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, char &value);
  static void bind_value(attribute_binder &binder, size_t index, char &value);
};

template <> struct data_type_traits<short, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_short; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, short &value);
  static void bind_value(attribute_binder &binder, size_t index, short &value);
};

template <> struct data_type_traits<int, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_int; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, int &value);
  static void bind_value(attribute_binder &binder, size_t index, int &value);
};

template <> struct data_type_traits<long, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_long; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, long &value);
  static void bind_value(attribute_binder &binder, size_t index, long &value);
};

template <> struct data_type_traits<long long, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_long_long; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, long long &value);
  static void bind_value(attribute_binder &binder, size_t index, long long &value);
};

template <> struct data_type_traits<unsigned char, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_unsigned_char; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, unsigned char &value);
  static void bind_value(attribute_binder &binder, size_t index, unsigned char &value);
};

template <> struct data_type_traits<unsigned short, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_unsigned_short; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, unsigned short &value);
  static void bind_value(attribute_binder &binder, size_t index, unsigned short &value);
};

template <> struct data_type_traits<unsigned int, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_unsigned_int; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, unsigned int &value);
  static void bind_value(attribute_binder &binder, size_t index, unsigned int &value);
};

template <> struct data_type_traits<unsigned long, void>
{
  inline static data_type type(std::size_t /*size*/ = 0) { return data_type::type_unsigned_long; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, unsigned long &value);
  static void bind_value(attribute_binder &binder, size_t index, unsigned long &value);
};

template <> struct data_type_traits<unsigned long long, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_unsigned_long_long; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, unsigned long long &value);
  static void bind_value(attribute_binder &binder, size_t index, unsigned long long &value);
};

template <> struct data_type_traits<bool, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_bool; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, bool &value);
  static void bind_value(attribute_binder &binder, size_t index, bool &value);
};

template <> struct data_type_traits<float, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_float; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, float &value);
  static void bind_value(attribute_binder &binder, size_t index, float &value);
};

template <> struct data_type_traits<double, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_double; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, double &value);
  static void bind_value(attribute_binder &binder, size_t index, double &value);
};

template <> struct data_type_traits<const char*, void>
{
  inline static data_type type(std::size_t size) { return size == 0 ? data_type::type_text : data_type::type_char_pointer; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, const char* value, size_t size);
  static void bind_value(attribute_binder &binder, size_t index, const char *value, size_t size = 0);
};

template <> struct data_type_traits<char*, void>
{
  inline static data_type type(std::size_t size) { return size == 0 ? data_type::type_text : data_type::type_varchar; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, char *value, size_t size);
  static void bind_value(attribute_binder &binder, size_t index, char *value, size_t size = 0);
};

template <> struct data_type_traits<char[], void>
{
  inline static data_type type(std::size_t size) { return size == 0 ? data_type::type_text : data_type::type_varchar; }
  template < int N >
  static void read_value(attribute_reader &reader, const char *id, size_t index, char (&value)[N], size_t size) {
    data_type_traits<const char*>::read_value(reader, id, index, value, size);
  }
  template < int N >
  static void bind_value(attribute_binder &binder, size_t index, char *value, size_t size = 0) {
    data_type_traits<const char*>::bind_value(binder, index, value, size);
  }
};

template <> struct data_type_traits<std::string, void>
{
  inline static data_type type(std::size_t size) { return size == 0 ? data_type::type_text : data_type::type_varchar; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, std::string &value, size_t size);
  static void bind_value(attribute_binder &binder, size_t index, std::string &value, size_t size = 0);
};

template <> struct data_type_traits<utils::blob, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_blob; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, utils::blob &value);
  static void bind_value(attribute_binder &binder, size_t index, utils::blob &value);
};

template <> struct data_type_traits<matador::date, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_date; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, matador::date &value);
  static void bind_value(attribute_binder &binder, size_t index, matador::date &value);
};

template <> struct data_type_traits<matador::time, void>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_time; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, matador::time &value);
  static void bind_value(attribute_binder &binder, size_t index, matador::time &value);
};

template < typename EnumType >
struct data_type_traits<EnumType, typename std::enable_if<std::is_enum<EnumType>::value>::type>
{
  inline static data_type type(std::size_t /*size*/) { return data_type::type_int; }
  static void read_value(attribute_reader &reader, const char *id, size_t index, EnumType &value)
  {
    data_type_traits<int>::read_value(reader, id, index, reinterpret_cast<int&>(value));
  }
  static void bind_value(attribute_binder &binder, size_t index, EnumType &value)
  {
    data_type_traits<int>::bind_value(binder, index, (int&)value);
  }
};
/// @endcond

}
#endif //QUERY_DATA_TYPE_TRAITS_HPP
