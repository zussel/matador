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

#ifdef _MSC_VER
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

#include "database/transaction.hpp"
#include "sql/result.hpp"

#include <string>
#include <stack>
#include <map>
#include <memory>

namespace oos {

class object_store;
class database;

/**
 * @class session
 * @brief Frontend class to make the objects persistent.
 * 
 * This class is the frontend to any sql made
 * available by a concrete sql implementation.
 * All objects in the given object_store will be made
 * persistent.
 */
class OOS_API session
{
public:
  /**
   * @brief Creates a session frontend for an object_store and a specific sql.
   * 
   * This constructor creates a connection between an object_store
   * and a specific sql identified by a sql connection
   * string.
   * 
   * @param ostore The object_store to make persistent.
   * @param dbstring The sql connection string.
   */
  session(object_store &ostore, const std::string &dbstring = "memory://");

  ~session();
  
  /**
   * @brief Opens the sql.
   * 
   * Opens the sql. If sql couldn't be opened
   * an exception is thrown.
   */
  void open();
  
  /**
   * @brief Returns true if sql is open.
   *
   * Returns true if sql is open
   *
   * @return True on open sql.
   */
  bool is_open() const;

  /**
   * @brief Creates the sql.
   * 
   * Try to create the sql and all tables described
   * in the object_store. If sql already exists an
   * exception is thrown.
   * Once the sql is created it is also opened.
   */
  void create();
  
  /**
   * @brief Drops all tables.
   *
   * When called all tables of the
   * sql are dropped and all data
   * is lost.
   */
  void drop();

  /**
   * @brief Closes the sql.
   * 
   * Closes the sql.
   */
  void close();

  /**
   * @cond OOS_DEV
   *
   * Load a concrete serializable of a specfic type
   * and a given id from the sql. If an serializable
   * with the given id couldn't be found an empty
   * object_ptr is returned
   *
   * @tparam T The type of the serializable.
   * @param id The unique primary id of the serializable.
   * @return The serializable defined by the given parameters.
   */
  template < class T >
  object_ptr<T> load(int /*id*/)
  {
    return object_ptr<T>();
  }

  /**
   * Load all objects of the given type
   * from the sql. If the operation
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
   * @brief Load all objects from the sql.
   *
   * Load all data into the object_store registered
   * objects from the sql. If the operation
   * succeeds true is returned.
   * If a table layoput doesn't match to the corresponding
   * objects layout an excpetion is thrown.
   *
   * @return Returns true on successful loading.
   */
  bool load();

  /**
   * @brief Executes a sql query.
   * 
   * Executes the given query on the sql and
   * return the result in a query_result serializable.
   * 
   * @param sql The sql query to execute.
   * @return The result of the query.
   */
  result<serializable> execute(const std::string &sql);

  /**
   * @brief Inserts a new serializable
   * @tparam T Type of serializable
   * 
   * Inserts a new not inserted serializable
   * into serializable store and persists
   * it on sql
   * 
   * @param o The serializable to insert.
   * @return The inserted persistent serializable.
   */
  template < class T >
  object_ptr<T> insert(T *o)
  {
    transaction tr(*this);
    tr.begin();
    object_ptr<T> optr = ostore_.insert(o);
    tr.commit();
    return optr;
  }

  /**
   * @brief Updates a given persistent serializable
   * @tparam T Type of serializable tp update.
   * 
   * Updates a persistent serializable in serializable store
   * and on sql.
   * 
   * @param optr The serializable to update
   */
  void update(const object_base_ptr &optr);

  /**
   * @brief Deletes a given persistent serializable
   * 
   * Deletes a persistent serializable in serializable store
   * and on sql.
   * 
   * @param optr The serializable to delete
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
   * session underlaying sql.
   *
   * @return The underlaying sql.
   */
  const database& db() const;

  /**
   * Returns a reference to the 
   * session underlaying sql.
   *
   * @return The underlaying sql.
   */
  database& db();

private:
  friend class transaction;
  template < class T >friend class statement;
  template <class T > friend class query;
  
  void push_transaction(transaction *tr);
  void pop_transaction();

  serializable * load(const std::string &type, int id = 0);

  void begin(transaction &tr);
  void commit(transaction &tr);
  void rollback();


  template < class T >
  void attach(const std::string &entity)
  {

  }

private:
  std::string type_;
  std::string connection_;

  database *impl_;

  object_store &ostore_;

  std::stack<transaction*> transaction_stack_;
};

}

#endif /* SESSION_HPP */
