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

#ifdef WIN32
#include <memory>
#else
#include <tr1/memory>
#endif

#include <map>

namespace oos {

class transaction;
class transaction_impl;
class statement_impl;

/**
 * @class database_impl
 * @brief Base class for all database backends
 * 
 * This class must be the base class for database
 * backends used with the object_store/database module.
 * The database actions are implemented using the
 * visitor pattern. So every action is accepted via
 * a method which must be overwritten by the concrete
 * database implementation.
 */
class OOS_API database_impl : public action_visitor
{
public:
  typedef std::tr1::shared_ptr<statement_impl> statement_impl_ptr;

public:
  database_impl();
  virtual ~database_impl();
  
  /**
   * Open the database
   */
  virtual void open(const std::string &db) = 0;

  /**
   * Close the database
   */
  virtual void close() = 0;

  /**
   * The interface for the create table action.
   */
  virtual void visit(create_action*) {}

  /**
   * The interface for the insert action.
   */
  virtual void visit(insert_action*) = 0;

  /**
   * The interface for the update action.
   */
  virtual void visit(update_action*) = 0;

  /**
   * The interface for the delete action.
   */
  virtual void visit(delete_action*) = 0;

  /**
   * The interface for the drop table action.
   */
  virtual void visit(drop_action*) {}

  /**
   * Create the concrete transaction_impl.
   *
   * @return The concrete transaction_impl.
   */
  transaction_impl* create_transaction(transaction &tr) const;

  /**
   * Create the concrete statement_impl.
   *
   * @return The concrete statement_impl.
   */
  virtual statement_impl* create_statement() = 0;

  /**
   * Store a prepared statement for
   * later use.
   * 
   * @param id A unique id for the statement.
   * @param stmt The prepared statement.
   * @return True if the statement could be stored
   */
  bool store_statement(const std::string &id, statement_impl_ptr stmt);

  /**
   * Find and return a stored statement.
   * 
   * @param id The id of the statement to find.
   * @return The requested statement.
   */
  statement_impl_ptr statement(const std::string &id) const;

private:
  typedef std::map<std::string, statement_impl_ptr> statement_impl_map_t;
  
  statement_impl_map_t statement_impl_map_;
};

}

#endif /* DATABASE_IMPL_HPP */
