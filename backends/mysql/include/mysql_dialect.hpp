#ifndef QUERY_POSTGRES_DIALECT_HPP
#define QUERY_POSTGRES_DIALECT_HPP

#ifdef _MSC_VER
#ifdef matador_mysql_EXPORTS
#define MATADOR_MYSQL_API __declspec(dllexport)
#else
#define MATADOR_MYSQL_API __declspec(dllimport)
#endif
#pragma warning(disable: 4355)
#else
#define MATADOR_MYSQL_API
#endif

#include "matador/sql/dialect.hpp"

extern "C" [[maybe_unused]] MATADOR_MYSQL_API const matador::sql::dialect* get_dialect();

#endif //QUERY_POSTGRES_DIALECT_HPP
