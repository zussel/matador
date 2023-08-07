#ifndef MATADOR_POSTGRESQL_CONNECTION_HPP
#define MATADOR_POSTGRESQL_CONNECTION_HPP

#ifdef _MSC_VER
#ifdef matador_postgresql_EXPORTS
    #define MATADOR_POSTGRESQL_API __declspec(dllexport)
  #else
    #define MATADOR_POSTGRESQL_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
  #pragma warning(disable: 4275)
#else
#define MATADOR_POSTGRESQL_API
#endif

#include "matador/sql/connection_impl.hpp"
#include "matador/sql/types.hpp"

#include "matador/db/postgresql/postgresql_dialect.hpp"

#include <libpq-fe.h>

namespace matador {

namespace postgresql {

class postgresql_statement;
class postgresql_result;

/**
 * @class mysql_database
 * @brief The mysql sql backend
 *
 * This class is the sqlite sql backend
 * class. It provides the mysql version 4 and higher
 */
class MATADOR_POSTGRESQL_API postgresql_connection : public connection_impl
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
  std::string client_version() const override;
  std::string server_version() const override;

  bool exists(const std::string &table_name) override;
  std::vector<field> describe(const std::string &table) override;

  basic_dialect* dialect() override;

  PGconn* handle() const;

private:
  postgresql_result* execute_internal(const std::string &stmt);

private:
  std::string db_;
  bool is_open_;
  postgresql_dialect dialect_;
  PGconn *conn_ = nullptr;
};

}

}

extern "C"
{
MATADOR_POSTGRESQL_API matador::connection_impl* create_database();

MATADOR_POSTGRESQL_API void destroy_database(matador::connection_impl *db);
}

#endif //MATADOR_POSTGRESQL_CONNECTION_HPP
