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

#ifndef DATABASE_HPP
#define DATABASE_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

#include "database/result.hpp"

#include "object/object_ptr.hpp"

#include "tools/library.hpp"

#include <string>
#include <stack>
#include <map>
#include <memory>

namespace oos {

class object_store;
class result;
class transaction;
class database_impl;
class statement_impl;

/**
 * @class database
 * @brief Frontend class to make the objects persistent.
 * 
 * This class is the frontend to any database made
 * available by a concrete database_impl implementation.
 * All objects in the given object_store will be made
 * persistent.
 */
class OOS_API database
{
public:
  /**
   * @brief Creates a database frontend for an object_store and a specific database.
   * 
   * This constructor creates a connection between an object_store
   * and a specific database identified by a database connection
   * string.
   * 
   * @param ostore The object_store to make persistent.
   * @param dbstring The database connection string.
   */
  database(object_store &ostore, const std::string &dbstring);

  ~database();
  
  /**
   * @brief Opens the database.
   * 
   * Opens the database. If database couldn't be opened
   * an exception is thrown.
   */
  void open();
  
  /**
   * @brief Creates the database.
   * 
   * Try to create the database and all tables described
   * in the object_store. If database already exists an
   * exception is thrown.
   * Once the database is created it is also opened.
   */
  void create();
  
  /**
   * @brief Closes the database.
   * 
   * Closes the database.
   */
  void close();

  /**
   * Load a concrete object of a specfic type
   * and a given id from the database. If an object
   * with the given id couldn't be found an empty
   * object_ptr is returned
   *
   * @tparam T The type of the object.
   * @param id The unique primary id of the object.
   * @return The object defined by the given parameters.
   */
  template < class T >
  object_ptr<T> load(int id)
  {
    return object_ptr<T>();
  }

  /**
   * Load all objects of the given type
   * from the database. If the operation
   * succeeds true is returned.
   *
   * @return Returns true on successful loading.
   */
  template < class T >
  bool load()
  {
    return true;
  }

  /**
   * @brief Load all objects from the database.
   *
   * Load all data into the object_store registered
   * objects from the database. If the operation
   * succeeds true is returned.
   * If a table layoput doesn't match to the corresponding
   * objects layout an excpetion is thrown.
   *
   * @return Returns true on successful loading.
   */
  bool load();

  /**
   * @brief Executes a database query.
   * 
   * Executes the given query on the database and
   * return the result in a query_result object.
   * 
   * @param sql The database query to execute.
   * @return The result of the query.
   */
  result_ptr execute(const std::string &sql);

  /**
   * Returns the object_store.
   */
  object_store& ostore();

  /**
   * Returns the object_store.
   */
  const object_store& ostore() const;

  /**
   * Return the current transaction from the stack.
   *
   * @return The current transaction.
   */
  transaction* current_transaction() const;

private:
  friend class transaction;
  friend class statement;
  friend class reader;
  friend class inserter;
  friend class updater;
  friend class deleter;
  
  void push_transaction(transaction *tr);
  void pop_transaction();

  object* load(const std::string &type, int id = 0);

  void begin(transaction &tr);
  void commit(transaction &tr);
  void rollback();

  /**
   * Create a statement implementation
   *
   * @return A statement_impl object.
   */
  statement_impl* create_statement_impl() const;

private:
  std::string type_;
  std::string connection_;

  database_impl *impl_;

  object_store &ostore_;

  std::stack<transaction*> transaction_stack_;
};

}

#endif /* DATABASE_HPP */
