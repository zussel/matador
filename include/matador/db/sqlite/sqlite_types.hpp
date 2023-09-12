#ifndef SQLITE_TYPES_HPP
#define SQLITE_TYPES_HPP

#include "matador/db/sqlite/sqlite_statement.hpp"

namespace matador {

class object_base_ptr;

namespace sqlite {

template <class T> 
struct type_traits; 

template <> struct type_traits<char> { inline static const char* type_string() { return "TINYINT"; } }; 
template <> struct type_traits<short> { inline static const char* type_string() { return "SMALLINT"; } };
template <> struct type_traits<int> { inline static const char* type_string() { return "INTEGER"; } };
template <> struct type_traits<long> { inline static const char* type_string() { return "BIGINT"; } };
template <> struct type_traits<long long> { inline static const char* type_string() { return "BIGINT"; } };
template <> struct type_traits<unsigned char> { inline static const char* type_string() { return "TINYINT"; } };
template <> struct type_traits<unsigned short> { inline static const char* type_string() { return "SMALLINT"; } };
template <> struct type_traits<unsigned int> { inline static const char* type_string() { return "INTEGER"; } };
template <> struct type_traits<unsigned long> { inline static const char* type_string() { return "BIGINT"; } };
template <> struct type_traits<unsigned long long> { inline static const char* type_string() { return "BIGINT"; } };
template <> struct type_traits<bool> { inline static const char* type_string() { return "BOOLEAN"; } };
template <> struct type_traits<float> { inline static const char* type_string() { return "FLOAT"; } };
template <> struct type_traits<double> { inline static const char* type_string() { return "DOUBLE"; } };
template <> struct type_traits<const char*> { inline static const char* type_string() { return "VARCHAR"; } };
template <> struct type_traits<std::string> { inline static const char* type_string() { return "VARCHAR"; } };
template <> struct type_traits<matador::date> { inline static const char* type_string() { return "DATE"; } };
template <> struct type_traits<matador::time> { inline static const char* type_string() { return "DATETIME"; } };
template <> struct type_traits<object_base_ptr> { inline static const char* type_string() { return "INTEGER"; } };

class sqlite_types
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

#endif /* SQLITE_TYPES_HPP */
