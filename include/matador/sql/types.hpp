#ifndef OOS_DATABASE_TYPES_HPP
#define OOS_DATABASE_TYPES_HPP

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include <string>

namespace matador {

/**
 * @brief Enumeration type of all builtin data types
 */
enum struct data_type {
  type_char = 0,           /*!< Data type char */
  type_short,              /*!< Data type short */
  type_int,                /*!< Data type int */
  type_long,               /*!< Data type long */
  type_long_long,          /*!< Data type long long */
  type_unsigned_char,      /*!< Data type unsigned char */
  type_unsigned_short,     /*!< Data type unsigned short */
  type_unsigned_int,       /*!< Data type unsigned int */
  type_unsigned_long,      /*!< Data type unsigned long */
  type_unsigned_long_long, /*!< Data type unsigned long long */
  type_float,              /*!< Data type float */
  type_double,             /*!< Data type double */
  type_bool,               /*!< Data type bool */
  type_char_pointer,       /*!< Data type character pointer */
  type_varchar,            /*!< Data type varchar */
  type_text,               /*!< Data type text */
  type_date,               /*!< Data type date */
  type_time,               /*!< Data type time */
  type_blob,               /*!< Data type blob */
  type_null,               /*!< Data type null */
  type_unknown             /*!< Data type unknown */
};
/**
 * @brief Enumeration of database data types
 */
enum struct database_type {
  type_char = 0,       /*!< Data type char */
  type_float,          /*!< Data type float */
  type_double,         /*!< Data type double */
  type_smallint,
  type_int,
  type_bigint,
  type_bool,           /*!< Data type bool */
  type_char_pointer,   /*!< Data type character pointer */
  type_varchar,        /*!< Data type varchar */
  type_text,           /*!< Data type text */
  type_date,           /*!< Data type date */
  type_time,           /*!< Data type time */
  type_blob,           /*!< Data type blob */
  type_null,           /*!< Data type null */
  type_unknown         /*!< Data type unknown */
};

/**
 * @tparam T The type of the traits
 * @brief Type traits for database types
 * 
 * This class is used to determine and
 * provide the correct size information
 * for a data type
 */
template < class T >
struct data_type_traits;

/// @cond MATADOR_DEV
template <> struct data_type_traits<char>
{
  inline static database_type type() { return database_type::type_char; }
  inline static data_type builtin_type() { return data_type::type_char; }
  inline static unsigned long size() { return sizeof(char); }
  inline static const char* name() { return "char"; }
};

template <> struct data_type_traits<short>
{
  inline static database_type type() { return database_type::type_smallint; }
  inline static data_type builtin_type() { return data_type::type_short; }
  inline static unsigned long size() { return sizeof(short); }
  inline static const char* name() { return "short"; }
};

template <> struct data_type_traits<int>
{
  inline static database_type type() { return database_type::type_int; }
  inline static data_type builtin_type() { return data_type::type_int; }
  inline static unsigned long size() { return sizeof(int); }
  inline static const char* name() { return "int"; }
};

template <> struct data_type_traits<long>
{
  inline static database_type type() { return database_type::type_bigint; }
  inline static data_type builtin_type() { return data_type::type_long; }
  inline static unsigned long size() { return sizeof(long); }
  inline static const char* name() { return "long"; }
};

template <> struct data_type_traits<long long>
{
  inline static database_type type() { return database_type::type_bigint; }
  inline static data_type builtin_type() { return data_type::type_long_long; }
  inline static unsigned long size() { return sizeof(long long); }
  inline static const char* name() { return "long long"; }
};

template <> struct data_type_traits<unsigned char>
{
  inline static database_type type() { return database_type::type_char; }
  inline static data_type builtin_type() { return data_type::type_unsigned_char; }
  inline static unsigned long size() { return sizeof(unsigned char); }
  inline static const char* name() { return "unsigned char"; }
};

template <> struct data_type_traits<unsigned short>
{
  inline static database_type type() { return database_type::type_smallint; }
  inline static data_type builtin_type() { return data_type::type_unsigned_short; }
  inline static unsigned long size() { return sizeof(unsigned short); }
  inline static const char* name() { return "unsigned short"; }
};

template <> struct data_type_traits<unsigned int>
{
  inline static database_type type() { return database_type::type_int; }
  inline static data_type builtin_type() { return data_type::type_unsigned_int; }
  inline static unsigned long size() { return sizeof(unsigned int); }
  inline static const char* name() { return "unsigned int"; }
};

template <> struct data_type_traits<unsigned long>
{
  inline static database_type type() { return database_type::type_bigint; }
  inline static data_type builtin_type() { return data_type::type_unsigned_long; }
  inline static unsigned long size() { return sizeof(unsigned long); }
  inline static const char* name() { return "unsigned long"; }
};

template <> struct data_type_traits<unsigned long long>
{
  inline static database_type type() { return database_type::type_bigint; }
  inline static data_type builtin_type() { return data_type::type_unsigned_long_long; }
  inline static unsigned long size() { return sizeof(unsigned long long); }
  inline static const char* name() { return "unsigned long long"; }
};

template <> struct data_type_traits<bool>
{
  inline static database_type type() { return database_type::type_bool; }
  inline static data_type builtin_type() { return data_type::type_bool; }
  inline static unsigned long size() { return sizeof(bool); }
  inline static const char* name() { return "bool"; }
};

template <> struct data_type_traits<float>
{
  inline static database_type type() { return database_type::type_float; }
  inline static data_type builtin_type() { return data_type::type_float; }
  inline static unsigned long size() { return sizeof(float); }
  inline static const char* name() { return "float"; }
};

template <> struct data_type_traits<double>
{
  inline static database_type type() { return database_type::type_double; }
  inline static data_type builtin_type() { return data_type::type_double; }
  inline static unsigned long size() { return sizeof(double); }
  inline static const char* name() { return "double"; }
};

template <> struct data_type_traits<const char*>
{
  inline static database_type type() { return database_type::type_varchar; }
  inline static data_type builtin_type() { return data_type::type_char_pointer; }
  inline static unsigned long size() { return sizeof(const char*); }
  inline static const char* name() { return "const char*"; }
};

template <> struct data_type_traits<std::string>
{
  inline static database_type type() { return database_type::type_text; }
  inline static data_type builtin_type() { return data_type::type_text; }
  inline static unsigned long size() { return 1024; }
  inline static const char* name() { return "std::string"; }
};

template <> struct data_type_traits<matador::date>
{
  inline static database_type type() { return database_type::type_date; }
  inline static data_type builtin_type() { return data_type::type_date; }
  inline static unsigned long size() { return 256; }
  inline static const char* name() { return "matador::date"; }
};

template <> struct data_type_traits<matador::time>
{
  inline static database_type type() { return database_type::type_time; }
  inline static data_type builtin_type() { return data_type::type_time; }
  inline static unsigned long size() { return 256; }
  inline static const char* name() { return "matador::time"; }
};

/// @endcond

}

#endif /* OOS_DATABASE_TYPES_HPP */
