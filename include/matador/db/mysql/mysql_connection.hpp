#ifndef MYSQL_DATABASE_HPP
#define MYSQL_DATABASE_HPP

#ifdef _MSC_VER
  #ifdef matador_mysql_EXPORTS
    #define OOS_MYSQL_API __declspec(dllexport)
  #else
    #define OOS_MYSQL_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
  #pragma warning(disable: 4275)
#else
  #define OOS_MYSQL_API
#endif

#include "matador/sql/connection_impl.hpp"
#include "matador/sql/types.hpp"

#include "matador/db/mysql/mysql_dialect.hpp"

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

namespace matador {
  
namespace mysql {

class mysql_statement;
class mysql_result;

/**
 * @class mysql_database
 * @brief The mysql sql backend
 * 
 * This class is the sqlite sql backend
 * class. It provides the mysql version 4 and higher
 */
class OOS_MYSQL_API mysql_connection : public connection_impl
{
public:
  mysql_connection();
  ~mysql_connection() override;
  
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
  MYSQL* handle();

  void open(const connection_info &info) override;
  void close() override;

  detail::result_impl* execute(const matador::sql &stmt) override;
  detail::result_impl* execute(const std::string &sql) override;
  detail::statement_impl* prepare(detail::statement_context &&context) const override;

  void begin() override;
  void commit() override;
  void rollback() override;

  std::string type() const override;
  version client_version() const override;
  version server_version() const override;

  bool exists(const std::string &table_name) override;
  std::vector<field> describe(const std::string &table) override;

  basic_dialect* dialect() override;

  unsigned short default_port() const override;

private:
  mysql_result* execute_internal(const std::string &stmt);

private:
  mutable MYSQL mysql_ = MYSQL();
  bool is_open_;
  mysql_dialect dialect_;
};

}

}

extern "C"
{
  OOS_MYSQL_API matador::connection_impl* create_database();

  OOS_MYSQL_API void destroy_database(matador::connection_impl *db);
}

#endif /* MYSQL_DATABASE_HPP */
