//
// Created by sascha on 24.05.19.
//

#ifndef MATADOR_POSTGRESQL_CONNECTION_HPP
#define MATADOR_POSTGRESQL_CONNECTION_HPP

#ifdef _MSC_VER
#ifdef matador_mysql_EXPORTS
    #define OOS_MYSQL_API __declspec(dllexport)
  #else
    #define OOS_MYSQL_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
#define OOS_MYSQL_API
#endif

#include "matador/sql/connection_impl.hpp"
#include "matador/sql/types.hpp"

#include "matador/db/postgresql/postgresql_dialect.hpp"

#ifdef _MSC_VER
//#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

namespace matador {

namespace postgresql {

class postgresql_statement;

/**
 * @class mysql_database
 * @brief The mysql sql backend
 *
 * This class is the sqlite sql backend
 * class. It provides the mysql version 4 and higher
 */
class OOS_MYSQL_API postgresql_connection : public connection_impl
{
public:
  postgresql_connection();
  ~postgresql_connection() override;

  /**
   * Returns true if the sql is open
   *
   * @return True on open sql connection.
   */
  bool is_open() const override;

  virtual unsigned long last_inserted_id();

  /**
   * Return the raw pointer to the sqlite3
   * database struct.
   *
   * @return The raw sqlite3 pointer.
   */
//  MYSQL* handle();

  void open(const std::string &db) override;
  void close() override;

  detail::result_impl* execute(const matador::sql &stmt) override;
  detail::result_impl* execute(const std::string &stmt) override;
  detail::statement_impl* prepare(const matador::sql &stmt) override;

  void begin() override;
  void commit() override;
  void rollback() override;

  std::string type() const override;
  std::string version() const override;

  bool exists(const std::string &tablename) override;
  std::vector<field> describe(const std::string &table) override;

  basic_dialect* dialect() override;

private:
  std::string db_;
  bool is_open_;
  postgresql_dialect dialect_;
};

}

}

extern "C"
{
OOS_MYSQL_API matador::connection_impl* create_database();

OOS_MYSQL_API void destroy_database(matador::connection_impl *db);
}

#endif //MATADOR_POSTGRESQL_CONNECTION_HPP
