#ifndef QUERY_ODBC_ERROR_HPP
#define QUERY_ODBC_ERROR_HPP

#ifdef _MSC_VER
#pragma warning(disable: 4355)
#endif

#if defined(_MSC_VER)
#include <WinSock2.h>
#endif

#include <sqltypes.h>

#include <string>

namespace matador::backends::odbc {

void throw_odbc_error(SQLRETURN ret, SQLSMALLINT htype, SQLHANDLE hndl, const std::string &source);
void throw_odbc_error(SQLRETURN ret, SQLSMALLINT htype, SQLHANDLE hndl, const std::string &source, const std::string &sql);

}

#endif //QUERY_ODBC_ERROR_HPP
