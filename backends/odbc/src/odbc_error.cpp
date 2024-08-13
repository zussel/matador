#include "odbc_error.hpp"

#include <sql.h>

#include <stdexcept>
#include <sstream>

namespace matador::backends::odbc {

void throw_odbc_error(SQLRETURN ret, SQLSMALLINT htype, SQLHANDLE hndl, const std::string &source)
{
  throw_odbc_error(ret, htype, hndl, source, "");
}

void throw_odbc_error(SQLRETURN ret, SQLSMALLINT htype, SQLHANDLE hndl, const std::string &source, const std::string &sql)
{
  if (SQL_SUCCEEDED(ret) || ret == SQL_NO_DATA) {
    return;
  }
  SQLCHAR state[6];
  SQLINTEGER error;
  SQLCHAR data[512];
  SQLSMALLINT over_by;

  ret = SQL_ERROR;
  SQLSMALLINT i = 0;
  ret = SQLGetDiagRec(htype, hndl, ++i, state, &error, data, 511, &over_by);
  if (ret == SQL_SUCCESS) {
    std::string what(reinterpret_cast<char*>(data));
    std::string sqlstate(reinterpret_cast<char*>(state));
    std::stringstream msg;
    if (sql.empty()) {
      msg << "odbc error (" << source << ") " << what << "(" << state << ")";
    } else {
      msg << "odbc error (" << source << ") " << what << "(" << state << "): " << sql;
    }
    throw std::logic_error(msg.str());
  }
}

}