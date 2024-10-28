#include "mysql_error.hpp"

#include <sstream>

namespace matador::backends::mysql {

sql::sql_error make_error(sql::sql_error_code ec, MYSQL *db, const std::string &sql)
{
  return sql::sql_error{ec,
                        std::to_string(mysql_errno(db)),
                        mysql_error(db),
                        "mysql",
                        sql};
}

sql::sql_error make_error(sql::sql_error_code ec, MYSQL_STMT *stmt, const std::string &sql)
{
  return sql::sql_error{ec,
                        std::to_string(mysql_stmt_errno(stmt)),
                        mysql_stmt_error(stmt),
                        "mysql",
                        sql};
}

sql::sql_error make_error(sql::sql_error_code ec, const std::string &msg, const std::string &sql)
{
  return sql::sql_error{ec, "", msg, "mysql", sql};
}

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