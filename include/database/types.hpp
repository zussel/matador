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

#include <string>

namespace oos {
  enum data_type_t {
    type_char = 0,
    type_short,
    type_int,
    type_long,
    type_unsigned_char,
    type_unsigned_short,
    type_unsigned_int,
    type_unsigned_long,
    type_float,
    type_double,
    type_bool,
    type_char_pointer,
    type_varchar,
    type_text,
    type_blob
  };

template < class T >
struct type_traits;

template <> struct type_traits<char>
{
  inline static data_type_t data_type() { return type_char; }
  inline static unsigned long type_size() { return sizeof(char); }
};

template <> struct type_traits<short>
{
  inline static data_type_t data_type() { return type_short; }
  inline static unsigned long type_size() { return sizeof(short); }
};

template <> struct type_traits<int>
{
  inline static data_type_t data_type() { return type_int; }
  inline static unsigned long type_size() { return sizeof(int); }
};

template <> struct type_traits<long>
{
  inline static data_type_t data_type() { return type_long; }
  inline static unsigned long type_size() { return sizeof(long); }
};

template <> struct type_traits<unsigned char>
{
  inline static data_type_t data_type() { return type_unsigned_char; }
  inline static unsigned long type_size() { return sizeof(unsigned char); }
};

template <> struct type_traits<unsigned short>
{
  inline static data_type_t data_type() { return type_unsigned_short; }
  inline static unsigned long type_size() { return sizeof(unsigned short); }
};

template <> struct type_traits<unsigned int>
{
  inline static data_type_t data_type() { return type_unsigned_int; }
  inline static unsigned long type_size() { return sizeof(unsigned int); }
};

template <> struct type_traits<unsigned long>
{
  inline static data_type_t data_type() { return type_unsigned_long; }
  inline static unsigned long type_size() { return sizeof(unsigned long); }
};

template <> struct type_traits<bool>
{
  inline static data_type_t data_type() { return type_bool; }
  inline static unsigned long type_size() { return sizeof(bool); }
};

template <> struct type_traits<float>
{
  inline static data_type_t data_type() { return type_float; }
  inline static unsigned long type_size() { return sizeof(float); }
};

template <> struct type_traits<double>
{
  inline static data_type_t data_type() { return type_double; }
  inline static unsigned long type_size() { return sizeof(double); }
};

/*
template <> struct type_traits<varchar_base>
{
  inline static data_type_t data_type() { return type_varchar; }
};
*/
template <> struct type_traits<const char*>
{
  inline static data_type_t data_type() { return type_varchar; }
  inline static unsigned long type_size() { return sizeof(const char*); }
};

template <> struct type_traits<std::string>
{
  inline static data_type_t data_type() { return type_text; }
  inline static unsigned long type_size() { return 1024; }
};

/*
template <> struct type_traits<object_base_ptr>
{
  inline static data_type_t data_type() { return type_long; }
};
*/

}

#endif /* OOS_DATABASE_TYPES_HPP */
