//
// Created by sascha on 24.05.19.
//

#ifndef MATADOR_POSTGRESQL_EXCEPTION_HPP
#define MATADOR_POSTGRESQL_EXCEPTION_HPP

#include <string>
#include <libpq-fe.h>

#include "matador/sql/sql_exception.hpp"

namespace matador {

namespace postgresql {

#define THROW_POSTGRESQL_ERROR(db, source, msg)         \
  do {                                                  \
    std::stringstream what;                             \
    what << msg;                                        \
    throw postgresql_exception(db, source, what.str()); \
  } while (false)

void throw_error(const std::string &source, const std::string &sql = "");

void throw_error(int ec, PGconn *db, const std::string &source, const std::string &sql = "");

//void throw_stmt_error(int ec, MYSQL_STMT *stmt, const std::string &source, const std::string &sql = "");

class postgresql_exception : public sql_exception
{
public:
  postgresql_exception(const std::string &source, const std::string &what);
  postgresql_exception(PGconn *db, const std::string &source, const std::string &what);

  ~postgresql_exception() noexcept override = default;
};

class postgresql_stmt_exception : public sql_exception
{
public:
  explicit postgresql_stmt_exception(const std::string &what);
//  postgresql_stmt_exception(MYSQL_STMT *stmt, const std::string &source, const std::string &what);

  ~postgresql_stmt_exception() noexcept override = default;
};

}

}

#endif //MATADOR_POSTGRESQL_EXCEPTION_HPP
