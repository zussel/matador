#ifndef MSSQL_EXCEPTION_HPP
#define MSSQL_EXCEPTION_HPP

#ifdef _MSC_VER
  #ifdef matador_mssql_EXPORTS
    #define OOS_MSSQL_API __declspec(dllexport)
  #else
    #define OOS_MSSQL_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_MSSQL_API
#endif

#include "matador/sql/sql_exception.hpp"

#if defined(_MSC_VER)
#include <WinSock2.h>
#endif

#include <sqltypes.h>

namespace matador {

namespace mssql {

void throw_database_error(SQLRETURN ret, SQLSMALLINT htype, SQLHANDLE hndl, const std::string &source, const std::string &sql = "");

bool is_success(SQLRETURN ret);

}

}

#endif /* MSSQL_EXCEPTION_HPP */
