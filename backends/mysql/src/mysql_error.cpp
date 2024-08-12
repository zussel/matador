#include "mysql_error.hpp"

#include <sstream>

namespace matador::backends::mysql {

void throw_mysql_error(const char *what, const std::string &source)
{
  std::stringstream msg;
  msg << "mysql error (" << source << "): " << what;
  throw std::logic_error(msg.str());
}

void throw_mysql_error(MYSQL *db, const std::string &source)
{
  if (mysql_errno(db) != 0) {
    throw_mysql_error(mysql_error(db), source);
  }
}

void throw_mysql_error(MYSQL_STMT *stmt, const std::string &source, const std::string &sql)
{
  if (mysql_stmt_errno(stmt) != 0) {
    std::stringstream msg;
    msg << "mysql error (" << source << ") " << mysql_stmt_error(stmt) << ": " << sql;
    throw std::logic_error(msg.str());
  }
}

}