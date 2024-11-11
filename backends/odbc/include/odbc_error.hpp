#ifndef QUERY_ODBC_ERROR_HPP
#define QUERY_ODBC_ERROR_HPP

#ifdef _MSC_VER
#pragma warning(disable: 4355)
#endif

#include "matador/sql/sql_error.hpp"

#if defined(_MSC_VER)
#include <WinSock2.h>
#endif

#include <sqltypes.h>

#include <string>

namespace matador::backends::odbc {

bool is_succeeded_or_no_data(SQLRETURN ret);
sql::sql_error make_error(sql::sql_error_code ec, const std::string& msg);
sql::sql_error make_error(sql::sql_error_code ec, SQLRETURN ret, SQLSMALLINT htype, SQLHANDLE hndl, const std::string& stmt = "");
// void throw_odbc_error(SQLRETURN ret, SQLSMALLINT htype, SQLHANDLE hndl, const std::string &source);
// void throw_odbc_error(SQLRETURN ret, SQLSMALLINT htype, SQLHANDLE hndl, const std::string &source, const std::string &sql);

}

#endif //QUERY_ODBC_ERROR_HPP
