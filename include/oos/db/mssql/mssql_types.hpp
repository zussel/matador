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

#ifndef MSSQL_TYPES_HPP
#define MSSQL_TYPES_HPP

#include "oos/db/mssql/mssql_statement.hpp"

namespace oos {

class varchar_base;

namespace mssql {

template <class T> 
struct type_traits; 

template <> struct type_traits<char> { inline static const char* type_string() { return "SMALLINT"; } }; 
template <> struct type_traits<short> { inline static const char* type_string() { return "SMALLINT"; } };
template <> struct type_traits<int> { inline static const char* type_string() { return "INT"; } };
template <> struct type_traits<long> { inline static const char* type_string() { return "BIGINT"; } };
template <> struct type_traits<unsigned char> { inline static const char* type_string() { return "SMALLINT"; } };
template <> struct type_traits<unsigned short> { inline static const char* type_string() { return "INT"; } };
template <> struct type_traits<unsigned int> { inline static const char* type_string() { return "BIGINT"; } };
template <> struct type_traits<unsigned long> { inline static const char* type_string() { return "NUMERIC(21,0)"; } };
template <> struct type_traits<bool> { inline static const char* type_string() { return "BIT"; } };
template <> struct type_traits<float> { inline static const char* type_string() { return "FLOAT"; } };
template <> struct type_traits<double> { inline static const char* type_string() { return "FLOAT"; } };
template <> struct type_traits<varchar_base> { inline static const char* type_string() { return "VARCHAR"; } };
template <> struct type_traits<const char*> { inline static const char* type_string() { return "VARCHAR"; } };
template <> struct type_traits<std::string> { inline static const char* type_string() { return "VARCHAR(MAX)"; } };
template <> struct type_traits<oos::date> { inline static const char* type_string() { return "DATE"; } };
template <> struct type_traits<oos::time> { inline static const char* type_string() { return "DATETIME"; } };

class mssql_types
{
public:
  static const char *create_postfix() { return "CREATE TABLE"; }
  static const char *select_postfix() { return "SELECT"; }
  static const char *insert_postfix() { return "INSERT INTO"; }
  static const char *update_postfix() { return "UPDATE"; }
  static const char *delete_postfix() { return "DELETE"; }
  static const char *drop_postfix() { return "DROP TABLE"; }

  static const char *primary_key_prefix() { return "PRIMARY KEY NOT NULL"; }

  template < class T >
  const char* type_string(const T&) const
  {
    return type_traits<T>::type_string();
  }
};

}

}

#endif /* MSSQL_TYPES_HPP */
