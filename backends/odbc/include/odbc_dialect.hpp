#ifndef QUERY_ODBC_DIALECT_HPP
#define QUERY_ODBC_DIALECT_HPP

#ifdef _MSC_VER
#ifdef matador_odbc_EXPORTS
#define MATADOR_ODBC_API __declspec(dllexport)
#else
#define MATADOR_ODBC_API __declspec(dllimport)
#endif
#pragma warning(disable: 4355)
#else
#define MATADOR_ODBC_API
#endif

#include "matador/sql/dialect.hpp"

extern "C" [[maybe_unused]] MATADOR_ODBC_API const matador::sql::dialect* get_dialect();

#endif //QUERY_ODBC_DIALECT_HPP
