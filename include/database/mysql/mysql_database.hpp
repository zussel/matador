/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MYSQL_DATABASE_HPP
#define MYSQL_DATABASE_HPP

#ifdef WIN32
  #ifdef oos_sqlite_EXPORTS
    #define OOS_MYSQL_API __declspec(dllexport)
  #else
    #define OOS_MYSQL_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_MYSQL_API
#endif

#include "database/database.hpp"

#include "database/mysql/mysql_sequencer.hpp"

struct st_mysql;

namespace oos {
  
struct prototype_node;

namespace mysql {

class mysql_statement;

/**
 * @class mysql_database
 * @brief The mysql database backend
 * 
 * This class is the sqlite database backend
 * class. It provides the mysql version 4 and higher
 */
class OOS_MYSQL_API mysql_database : public database
{
public:
  explicit mysql_database(session *db);
  virtual ~mysql_database();
  
  virtual void open(const std::string &db);

  /**
   * Returns true if the database is open
   *
   * @return True on open database connection.
   */
  virtual bool is_open() const;

  /**
   * Close the database
   */
  virtual void close();

  /**
   * Execute a sql statement and return a result
   * implementation via pointer.
   *
   * @param sql The sql statement to be executed.
   */
  virtual void execute(const char *sql, result_impl *res = 0);

  /**
   * Create a new sqlite result
   * 
   * @return A new sqlite result
   */
  virtual result_impl* create_result();

  /**
   * Create a new sqlite statement
   * 
   * @return A new sqlite statement
   */
  virtual statement* create_statement();
  
  /**
   * Creates a new table for the given
   * prototype node.
   * 
   * @param node The prototype node.
   * @return The new table based on the prototype.
   */
  virtual table* create_table(const prototype_node &node);

  /**
   * Return the raw pointer to the sqlite3
   * database struct.
   * 
   * @return The raw sqlite3 pointer.
   */
  st_mysql* operator()();

protected:
  virtual void on_begin();
  virtual void on_commit();
  virtual void on_rollback();

private:
  static int parse_result(void* param, int column_count, char** values, char** columns);

private:
  st_mysql *mysql_db_;
};

}

}

extern "C"
{
  OOS_MYSQL_API oos::database* create_database(oos::session *ses);

  OOS_MYSQL_API void destroy_database(oos::database *db);
}

#endif /* MYSQL_DATABASE_HPP */
