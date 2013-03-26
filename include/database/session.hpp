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

#ifndef SESSION_HPP
#define SESSION_HPP

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

#include "object/object_ptr.hpp"
#include "object/object_store.hpp"

#include "tools/library.hpp"

#include "database/database.hpp"

#include <string>
#include <stack>
#include <map>
#include <memory>

namespace oos {

class object_store;
class result;
class transaction;
class statement;

/**
 * @class session
 * @brief Frontend class to make the objects persistent.
 * 
 * This class is the frontend to any database made
 * available by a concrete database implementation.
 * All objects in the given object_store will be made
 * persistent.
 */
class OOS_API session
{
public:
  /**
   * @brief Creates a session frontend for an object_store and a specific database.
   * 
   * This constructor creates a connection between an object_store
   * and a specific database identified by a database connection
   * string.
   * 
   * @param ostore The object_store to make persistent.
   * @param dbstring The database connection string.
   */
  session(object_store &ostore, const std::string &dbstring = "memory://");

  ~session();
  
  /**
   * @brief Opens the database.
   * 
   * Opens the database. If database couldn't be opened
   * an exception is thrown.
   */
  void open();
  
  /**
   * @brief Returns true if database is open.
   *
   * Returns true if database is open
   *
   * @return True on open database.
   */
  bool is_open() const;

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
   * @brief Drops all tables.
   *
   * When called all tables of the
   * database are dropped and all data
   * is lost.
   */
  void drop();

  /**
   * @brief Closes the database.
   * 
   * Closes the database.
   */
  void close();

  /**
   * @cond OOS_DEV
   *
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
  /// @endcond

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
  result* execute(const std::string &sql);

  /**
   * @brief Inserts a new object
   * @tparam T Type of object
   * 
   * Inserts a new not inserted object
   * into object store and persists
   * it on database
   * 
   * @param o The object to insert.
   * @return The inserted persistent object.
   */
  template < class T >
  object_ptr<T> insert(T *o)
  {
    transaction tr(*this);
    try {
      tr.begin();
      object_ptr<T> optr = ostore_.insert(o);
      tr.commit();
      return optr;
    } catch (std::exception &) {
      return object_ptr<T>();
    }
  }

  /**
   * @brief Updates a given persistent object
   * @tparam T Type of object tp update.
   * 
   * Updates a persistent object in object store
   * and on database.
   * 
   * @param optr The object to update
   */
  void update(const object_base_ptr &optr);

  /**
   * @brief Deletes a given persistent object
   * 
   * Deletes a persistent object in object store
   * and on database.
   * 
   * @param optr The object to delete
   */
  void remove(object_base_ptr &optr);

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

  /**
   * Returns a constant reference to the 
   * session underlaying database.
   *
   * @return The underlaying database.
   */
  const database& db() const;

  /**
   * Returns a reference to the 
   * session underlaying database.
   *
   * @return The underlaying database.
   */
  database& db();

private:
  friend class transaction;
  friend class statement;
  friend class query;
  
  void push_transaction(transaction *tr);
  void pop_transaction();

  object* load(const std::string &type, int id = 0);

  void begin(transaction &tr);
  void commit(transaction &tr);
  void rollback();

  /**
   * Create a statement implementation
   *
   * @return A statement object.
   */
  statement* create_statement() const;

private:
  std::string type_;
  std::string connection_;

  database *impl_;

  object_store &ostore_;

  std::stack<transaction*> transaction_stack_;
};

}

#endif /* SESSION_HPP */
