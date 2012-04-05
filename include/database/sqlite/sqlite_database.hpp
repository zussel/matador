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
   * Create a sqlite table from the given object.
   *
   * @param o The object providing the table layout.
   */
  virtual void create(const prototype_node &node);

  /**
   * load a specific table based on
   * a prototype node
   *
   * @param node The node representing the table to read
   */
  virtual void load(const prototype_node &node);

  /**
   * Execute a sql statement and return a result
   * implementation via pointer.
   *
   * @param sql The sql statement to be executed.
   */
  virtual void execute(const char *sql, result_impl *res = 0);

  virtual void visit(create_action *) {}

  /**
   * The interface for the insert action.
   */
  virtual void visit(insert_action *a);

  /**
   * The interface for the update action.
   */
  virtual void visit(update_action *a);

  /**
   * The interface for the delete action.
   */
  virtual void visit(delete_action *a);  

  /**
   * The interface for the drop table action.
   */
  virtual void visit(drop_action *) {}

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
  virtual statement_impl* create_statement();

  /**
   * Return the raw pointer to the sqlite3
   * database struct.
   * 
   * @return The raw sqlite3 pointer.
   */
  sqlite3* operator()();

protected:
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

#endif /* SQLITE_DATABASE_HPP */
