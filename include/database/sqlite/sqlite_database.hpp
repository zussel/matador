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

#ifndef SQLITE_DATABASE_HPP
#define SQLITE_DATABASE_HPP

#ifdef WIN32
  #ifdef oos_sqlite_EXPORTS
    #define OOS_SQLITE_API __declspec(dllexport)
  #else
    #define OOS_SQLITE_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_SQLITE_API
#endif

#include "database/database.hpp"

#include "database/sqlite/sqlite_sequencer.hpp"

struct sqlite3;

namespace oos {
  
struct prototype_node;

namespace sqlite {

class sqlite_statement;

/**
 * @class sqlite_database
 * @brief The sqlite database backend
 * 
 * This class is the sqlite database backend
 * class. It provides the sqlite version 3
 */
class OOS_SQLITE_API sqlite_database : public database
{
public:
  explicit sqlite_database(session *db);
  virtual ~sqlite_database();
  
  /**
   * Returns true if the database is open
   *
   * @return True on open database connection.
   */
  virtual bool is_open() const;

  /**
   * Execute a sql statement and return a result
   * implementation via pointer.
   *
   * @param sql The sql statement to be executed.
   */
  virtual result* execute(const std::string &sql);

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
  sqlite3* operator()();

  virtual const char* type_string(sql::data_type_t type) const;

protected:
  virtual void on_open(const std::string &db);
  virtual void on_close();
  virtual void on_begin();
  virtual void on_commit();
  virtual void on_rollback();

private:
  static int parse_result(void* param, int column_count, char** values, char** columns);

private:
  sqlite3 *sqlite_db_;
};

}

}

extern "C"
{
  OOS_SQLITE_API oos::database* create_database(oos::session *ses);

  OOS_SQLITE_API void destroy_database(oos::database *db);
}

#endif /* SQLITE_DATABASE_HPP */
