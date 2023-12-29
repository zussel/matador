#ifndef SQLITE_DATABASE_HPP
#define SQLITE_DATABASE_HPP

#ifdef _MSC_VER
  #ifdef matador_sqlite_EXPORTS
    #define OOS_SQLITE_API __declspec(dllexport)
  #else
    #define OOS_SQLITE_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_SQLITE_API
#endif

#include "matador/db/sqlite/sqlite_dialect.hpp"

#include "matador/sql/statement_impl.hpp"
#include "matador/sql/connection_impl.hpp"
#include "matador/sql/result_impl.hpp"

struct sqlite3;

namespace matador {
  
class prototype_node;

namespace sqlite {

class sqlite_statement;
class sqlite_result;

/**
 * @class sqlite_database
 * @brief The sqlite sql backend
 * 
 * This class is the sqlite sql backend
 * class. It provides the sqlite version 3
 */
//class OOS_SQLITE_API sqlite_connection : public connection_impl
class sqlite_connection : public connection_impl
{
public:
  sqlite_connection();
  ~sqlite_connection() override;
  
  /**
   * Returns true if the sql is open
   *
   * @return True on open sql connection.
   */
  bool is_open() const override;

  unsigned long last_inserted_id();

  void open(const connection_info &info) override;
  void close() override;

  detail::result_impl* execute(const sql &stmt) override;
  detail::result_impl* execute(const std::string &stmt) override;
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

  /**
   * Return the raw pointer to the sqlite3
   * database struct.
   * 
   * @return The raw sqlite3 pointer.
   */
  sqlite3* handle();

private:
  sqlite_result* execute_internal(const std::string &stmt);

  static int parse_result(void* param, int column_count, char** values, char** columns);

private:
  sqlite3 *sqlite_db_;
  sqlite_dialect dialect_;
};

}

}

extern "C"
{
  OOS_SQLITE_API matador::connection_impl* create_database();

  OOS_SQLITE_API void destroy_database(matador::connection_impl *db);
}

#endif /* SQLITE_DATABASE_HPP */
