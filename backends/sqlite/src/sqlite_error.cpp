#include "sqlite_error.hpp"

#include <stdexcept>
#include <sstream>

#include <sqlite3.h>

namespace matador::backends::sqlite {

void throw_sqlite_error(int ec, sqlite3 *db, const std::string &source)
{
  if (ec != SQLITE_OK && ec != SQLITE_DONE) {
    std::stringstream msg;
    msg << "sqlite error (" << source << "): " << sqlite3_errmsg(db);
    throw std::logic_error(msg.str());
  }
}

void throw_sqlite_error(int ec, sqlite3 *db, const std::string &source, const std::string &sql)
{
  if (ec != SQLITE_OK&& ec != SQLITE_DONE) {
    std::stringstream msg;
    msg << "sqlite error (" << source << ", sql: " << sql << "): " << sqlite3_errmsg(db);
    throw std::logic_error(msg.str());
  }
}

}