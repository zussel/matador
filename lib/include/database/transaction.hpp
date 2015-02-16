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

#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

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

#include "object/object_observer.hpp"

#include "tools/byte_buffer.hpp"

#include <unordered_map>
#include <memory>
#include <list>
#include <set>
#include <map>

namespace oos {

class session;
class object_store;
class object_proxy;
class byte_buffer;
class action;

/**
 * @class transaction
 * @brief The transaction class
 *
 * This class provides transaction functionality
 * for the object_store with a specific database.
 * If the object_store uses a database backend this
 * transaction class handles the commit and rollback
 * behaviour of the database. On rollback it restores
 * the stored data to the objects modified within
 * the transaction.
 */
class OOS_API transaction : public object_observer
{
public:
  typedef std::list<action*> action_list_t;             /**< Shortcut for the action list class type. */
  typedef action_list_t::iterator iterator;             /**< Shortcut for the action list iterator. */
  typedef action_list_t::const_iterator const_iterator; /**< Shortcut for the action list const iterator. */

public:
  /**
   * @brief Create a transaction
   *
   * A transaction for the given database
   * is created. To begin the transaction
   * start must be called.
   *
   * @param db The underlaying database.
   */
  transaction(session &db);

  ~transaction();
  
  /**
   * Return the unique transaction id.
   *
   * @return The transaction id.
   */
  long id() const;

  /**
   * @brief Start the transaction.
   *
   * Start the transaction. All object insertions,
   * modifications and deletions are stored.
   */
  void begin();

  /**
   * @brief Commit the started transaction.
   *
   * Commit the started transaction. All object
   * insertions, modifications and deletions are
   * written to the database.
   */
  void commit();

  /**
   * @brief Abort and rollback the started transaction.
   *
   * Abort and rollback the started transaction. All
   * object insertions, modifications and deletions are
   * rolled back in the object_store.
   */
  void rollback();

  /**
   * Returns the underlaying pointer to the database.
   *
   * @return The pointer to the database.
   */
  session& db();

  /**
   * Returns the underlaying pointer to the database.
   *
   * @return The pointer to the database.
   */
  const session& db() const;

  virtual void on_insert(object_proxy *proxy);
  virtual void on_update(object_proxy *proxy);
  virtual void on_delete(object_proxy *proxy);

private:
  typedef std::set<long> id_set_t;
  typedef std::unordered_map<long, iterator> id_iterator_map_t;

  friend class object_store;
  friend class session;
  
  void backup(action *a, const object *o);
  void restore(action *a);

  void cleanup();

private:
  static long id_counter;

private:
  session &db_;
  long id_;
  
  id_iterator_map_t id_map_;
  action_list_t action_list_;

  byte_buffer object_buffer_;
};

}

#endif /* TRANSACTION_HPP */
