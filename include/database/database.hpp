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

#ifndef DATABASE_IMPL_HPP
#define DATABASE_IMPL_HPP

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

#include "database/action.hpp"
#include "database/transaction.hpp"

#include "tools/sequencer.hpp"

#ifdef WIN32
#include <memory>
#include <unordered_map>
#else
#include <tr1/memory>
#include <tr1/unordered_map>
#endif

#include <map>
#include <list>

namespace oos {

class transaction;
class session;
class statement;
class table;
class result_impl;
class database_sequencer;
struct prototype_node;

/// @cond OOS_DEV
/**
 * @class database
 * @brief Base class for all database backends
 * 
 * This class must be the base class for database
 * backends used with the object_store/database module.
 * The database actions are implemented using the
 * visitor pattern. So every action is accepted via
 * a method which must be overwritten by the concrete
 * database implementation.
 */
class OOS_API database : public action_visitor
{
public:
  typedef std::list<object*> object_list_t;
  typedef std::tr1::unordered_map<long, object_list_t> object_map_t;
  typedef std::map<std::string, object_map_t> relation_data_t;
  typedef std::tr1::shared_ptr<statement> statement_ptr;
  typedef std::tr1::shared_ptr<table> table_ptr;
  typedef std::tr1::shared_ptr<database_sequencer> database_sequencer_ptr;

  struct table_info_t
  {
    table_info_t(const prototype_node *n) : is_loaded(false), node(n) {}
    bool is_loaded;
    relation_data_t relation_data;
    const prototype_node *node;
  };

protected:
  database(session *db, database_sequencer *seq);

public:
  virtual ~database();
  
  /**
   * Open the database
   *
   * @param db The database connection string.
   */
  virtual void open(const std::string &db);

  /**
   * Close the database
   */
  virtual void close();

  /**
   * Returns true if the database is open
   *
   * @return True on open database connection.
   */
  virtual bool is_open() const = 0;

  /**
   * Create all tables.
   */
  virtual void create();

  /**
   * Create a table from the given object.
   *
   * @param o The object providing the table layout.
   */
  virtual void create(const prototype_node &node);

  /**
   * Drops table defined by the given
   * prototype_node from the database.
   *
   * @param o The object providing the table layout.
   */
  virtual void drop(const prototype_node &node);

  /**
   * Drop all tables.
   */
  virtual void drop();

  /**
   * load a specific table based on
   * a prototype node
   *
   * @param node The node representing the table to read
   */
  virtual void load(const prototype_node &node);

  /**
   * Checks if a specific table was loaded.
   * 
   * @param name Name of the table to check.
   * @return Returns true if the table was loaded.
   */
  bool is_loaded(const std::string &name) const;

  /**
   * Execute a sql statement and return a result
   * implementation via pointer.
   *
   * @param sql The sql statement to be executed.
   */
  virtual void execute(const char *sql, result_impl *res = 0) = 0;

  /**
   * The interface for the create table action.
   */
  virtual void visit(create_action*) {}

  /**
   * The interface for the insert action.
   */
  virtual void visit(insert_action*);

  /**
   * The interface for the update action.
   */
  virtual void visit(update_action*);

  /**
   * The interface for the delete action.
   */
  virtual void visit(delete_action*);

  /**
   * The interface for the drop table action.
   */
  virtual void visit(drop_action*) {}

  /**
   * Create a new result object
   * 
   * @return New result implenation object.
   */
  virtual result_impl* create_result() = 0;

  /**
   * Create the concrete statement.
   *
   * @return The concrete statement.
   */
  virtual statement* create_statement() = 0;

  /**
   * Store a prepared statement for
   * later use.
   * 
   * @param id A unique id for the statement.
   * @param stmt The prepared statement.
   * @return True if the statement could be stored
   */
//  bool store_statement(const std::string &id, statement_ptr stmt);

  /**
   * Find and return a stored statement.
   * 
   * @param id The id of the statement to find.
   * @return The requested statement.
   */
//  statement_ptr find_statement(const std::string &id) const;

  virtual void initialize_table(const prototype_node &node,
                         std::string &create_, std::string &drop_) = 0;

  virtual void prepare_table(const prototype_node &node,
                         statement *select, statement *insert,
                         statement *update, statement *remove) = 0;
  /**
   * @brief Prepares the beginning of a transaction
   * 
   * Prepares the begin of the transaction
   * in database and object store. Calls
   * begin() on sequencer, which backups
   * object stores current ids.
   */
  void prepare();

  /**
   * @brief Marks the beginning of the real transaction.
   * 
   * This method is called when the real database
   * transaction are about to begin and a BEGIN
   * TRANSACTION kind of statement is executed on
   * the database.
   */
  void begin();

  /**
   * @brief Called on transaction commit
   *
   * This method is called when a started
   * transaction is commit to the underlaying
   * database. All stored actions and their
   * objects are written to the database.
   */
  void commit();

  /**
   * @brief Rolls back the current transaction
   * 
   * When called the current transaction is
   * rolled back on database and subsequently
   * in object store. All transient data is
   * restored.
   */
  void rollback();

protected:
  const session* db() const;

  session* db();

  virtual void on_begin() = 0;
  virtual void on_commit() = 0;
  virtual void on_rollback() = 0;


private:
  friend class database_factory;
  friend class table;

  session *db_;
  bool commiting_;

  typedef std::map<std::string, table_ptr> table_map_t;
  
  table_map_t table_map_;

  database_sequencer_ptr sequencer_;
  sequencer_impl_ptr sequencer_backup_;
};

/// @endcond

}

#endif /* DATABASE_IMPL_HPP */
