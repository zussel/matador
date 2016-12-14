/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OOS_DATABASE_TYPES_HPP
#define OOS_DATABASE_TYPES_HPP

#ifdef _MSC_VER
#ifdef oos_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_SQL_API
#endif

#include "oos/utils/date.hpp"
#include "oos/utils/time.hpp"
#include "oos/utils/varchar.hpp"

#include <string>

namespace oos {

/**
 * Enumeration of database data types
 */
enum struct data_type {
  type_char = 0,       /*!< Data type char */
  type_short,          /*!< Data type short */
  type_int,            /*!< Data type int */
  type_long,           /*!< Data type long */
  type_unsigned_char,  /*!< Data type unsigned char */
  type_unsigned_short, /*!< Data type unsigned short */
  type_unsigned_int,   /*!< Data type unsigned int */
  type_unsigned_long,  /*!< Data type unsigned long */
  type_float,          /*!< Data type float */
  type_double,         /*!< Data type double */
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
 * @brief Output operator for data types
 *
 * @param out The output strream to write on
 * @param type The data ytpe to be written
 * @return The modified output stream
 */
OOS_SQL_API std::ostream& operator<<(std::ostream &out, const oos::data_type type);

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

/// @cond OOS_DEV
template <> struct data_type_traits<char>
{
  inline static data_type type() { return data_type::type_char; }
  inline static unsigned long size() { return sizeof(char); }
};

template <> struct data_type_traits<short>
{
  inline static data_type type() { return data_type::type_short; }
  inline static unsigned long size() { return sizeof(short); }
};

template <> struct data_type_traits<int>
{
  inline static data_type type() { return data_type::type_int; }
  inline static unsigned long size() { return sizeof(int); }
};

template <> struct data_type_traits<long>
{
  inline static data_type type() { return data_type::type_long; }
  inline static unsigned long size() { return sizeof(long); }
};

template <> struct data_type_traits<unsigned char>
{
  inline static data_type type() { return data_type::type_unsigned_char; }
  inline static unsigned long size() { return sizeof(unsigned char); }
};

template <> struct data_type_traits<unsigned short>
{
  inline static data_type type() { return data_type::type_unsigned_short; }
  inline static unsigned long size() { return sizeof(unsigned short); }
};

template <> struct data_type_traits<unsigned int>
{
  inline static data_type type() { return data_type::type_unsigned_int; }
  inline static unsigned long size() { return sizeof(unsigned int); }
};

template <> struct data_type_traits<unsigned long>
{
  inline static data_type type() { return data_type::type_unsigned_long; }
  inline static unsigned long size() { return sizeof(unsigned long); }
};

template <> struct data_type_traits<bool>
{
  inline static data_type type() { return data_type::type_bool; }
  inline static unsigned long size() { return sizeof(bool); }
};

template <> struct data_type_traits<float>
{
  inline static data_type type() { return data_type::type_float; }
  inline static unsigned long size() { return sizeof(float); }
};

template <> struct data_type_traits<double>
{
  inline static data_type type() { return data_type::type_double; }
  inline static unsigned long size() { return sizeof(double); }
};

template < unsigned int C >
struct data_type_traits<varchar<C>>
{
  inline static data_type type() { return data_type::type_varchar; }
  inline static unsigned long size() { return C; }
};

template <> struct data_type_traits<const char*>
{
  inline static data_type type() { return data_type::type_varchar; }
  inline static unsigned long size() { return sizeof(const char*); }
};

template <> struct data_type_traits<std::string>
{
  inline static data_type type() { return data_type::type_text; }
  inline static unsigned long size() { return 1024; }
};

template <> struct data_type_traits<oos::date>
{
  inline static data_type type() { return data_type::type_date; }
  inline static unsigned long size() { return 256; }
};

template <> struct data_type_traits<oos::time>
{
  inline static data_type type() { return data_type::type_time; }
  inline static unsigned long size() { return 256; }
};

/*
template <> struct data_type_traits<object_base_ptr>
{
  inline static data_type data_type() { return type_long; }
};
*/

/// @endcond

}

#endif /* OOS_DATABASE_TYPES_HPP */
