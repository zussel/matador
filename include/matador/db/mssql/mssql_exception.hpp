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

//void throw_error(SQLRETURN ret, SQLSMALLINT htype, SQLHANDLE hndl, const std::string &source, const std::string &sql = "");

//void throw_error(const std::string &source, const std::string &sql = "");

bool is_success(SQLRETURN ret);

}

}

#endif /* MSSQL_EXCEPTION_HPP */
