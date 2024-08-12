#include "odbc_error.hpp"

#include <stdexcept>
#include <sstream>

namespace matador::backends::odbc {

void throw_odbc_error(int ec, sqlite3 *db, const std::string &source)
{
}

void throw_odbc_error(int ec, sqlite3 *db, const std::string &source, const std::string &sql)
{
}

}