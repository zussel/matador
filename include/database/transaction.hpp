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

#include "object/object_serializer.hpp"
#include "object/object_observer.hpp"

#include "database/action.hpp"

#include "tools/byte_buffer.hpp"

#include <memory>
#include <list>
#include <set>
#include <map>

namespace oos {

class database;
class transaction;
class object_store;
class byte_buffer;

/**
 * @class transaction_impl
 * @brief The concrete transaction implementation
 *
 * This class implements depending on the concrete database
 * implementation the transaction backend. It deals with
 * how the transaction commit and rollback take place for a
 * specific database.
 * In most cases the default implementation fits all needs.
 */
class OOS_API transaction_impl
{
public:
  /**
   * Creates the transaction implementation
   * for a transaction.
   *
   * @param tr The transaction.
   */
  transaction_impl(transaction &tr);

  virtual ~transaction_impl();
  
  /**
   * @brief Called on transaction rollback
   *
   * This method is called when a rollback
   * for a transaction takes place. All stored
   * actions and their objects are restored
   * to their backuped state.
   */
  virtual void rollback();

  /**
   * @brief Called on transaction commit
   *
   * This method is called when a started
   * transaction is commit to the underlaying
   * database. All stored actions and their
   * objects are written to the database.
   */
  virtual void commit();

private:
  transaction &tr_;
};

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
class OOS_API transaction
{
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
  transaction(database *db);

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
  void start();

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
  database* db();

  /**
   * Returns the underlaying pointer to the database.
   *
   * @return The pointer to the database.
   */
  const database* db() const;

private:
  class transaction_observer : public object_observer
  {
  public:
    transaction_observer(transaction &tr)
      : tr_(tr)
    {}
    virtual ~transaction_observer() {}

    virtual void on_insert(object *o);
    virtual void on_update(object *o);
    virtual void on_delete(object *o);
  private:
    transaction &tr_;
  };

  class backup_visitor : public action_visitor
  {
  public:
    backup_visitor()
      : buffer_(NULL)
    {}
    virtual ~backup_visitor() {}

    bool backup(action *act, byte_buffer *buffer);

    virtual void visit(create_action *) {}
    virtual void visit(insert_action *a);
    virtual void visit(update_action *a);
    virtual void visit(delete_action *a);  
    virtual void visit(drop_action *) {}

  private:
    byte_buffer *buffer_;
    object_serializer serializer_;
  };

  class restore_visitor : public action_visitor
  {
  public:
    restore_visitor()
      : buffer_(NULL)
      , ostore_(NULL)
    {}
    virtual ~restore_visitor() {}

    bool restore(action *act, byte_buffer *buffer, object_store *ostore);

    virtual void visit(create_action *) {}
    virtual void visit(insert_action *a);
    virtual void visit(update_action *a);
    virtual void visit(delete_action *a);
    virtual void visit(drop_action *) {}

  private:
    byte_buffer *buffer_;
    object_store *ostore_;
    object_serializer serializer_;
  };

private:
  typedef std::set<long> id_set_t;

  typedef std::list<action*> action_list_t;
  typedef action_list_t::iterator iterator;
  typedef action_list_t::const_iterator const_iterator;

  typedef std::map<std::string, action_list_t> insert_action_map_t;

  friend class transaction_impl;
  friend class object_store;
  friend class database;
  
  void backup(insert_action *ia);
  void backup(action *a);
  void restore(action *a);

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;

  iterator erase(iterator i);

  bool empty() const;
  size_t size() const;

  bool has_id(long id) const;

  const action* find_insert_action(object *o) const;
  void erase_insert_action(const action* a);

  iterator find_modify_action(object *o);

private:
  static long id_counter;

private:
  database *db_;
  transaction_observer observer_;
  long id_;
  
  transaction_impl *impl_;
  
  id_set_t id_set_;
  action_list_t action_list_;

  insert_action_map_t insert_action_map_;

  byte_buffer object_buffer_;
  backup_visitor backup_visitor_;
  restore_visitor restore_visitor_;
};

}

#endif /* TRANSACTION_HPP */
