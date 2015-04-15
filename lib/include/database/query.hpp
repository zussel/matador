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

#ifndef QUERY_HPP
#define QUERY_HPP

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

#include "database/sql.hpp"

#include <memory>
#include <sstream>
#include <object/object_ptr.hpp>

namespace oos {

class result;
class session;
class statement;
class database;
class object;
class prototype_node;
class condition;
class serializable;

/**
 * @class query
 * @brief Creates a SQL query.
 * 
 * The query class represents a SQL
 * query for any database provided
 * by the backend driver libraries.
 * 
 * With this class at hand you can create
 * - create
 * - drop
 * - select
 * - insert
 * - update 
 * - delete
 *
 * statements. These statements can be
 * created manually by adding each part
 * of the statement by hand or you can
 * create them using object based class or
 * instances.
 * 
 * The class uses a chain mechanism. Each method
 * representing a part of a query returns the reference
 * to itself. So you can concatenate your query
 * parts by calling the methods in a chain
 * (concatenated by dots).
 */
class OOS_API query
{
private:
  enum state_t {
    QUERY_BEGIN,
    QUERY_CREATE,
    QUERY_DROP,
    QUERY_OBJECT_SELECT,
    QUERY_OBJECT_INSERT,
    QUERY_OBJECT_UPDATE,
    QUERY_SELECT,
    QUERY_INSERT,
    QUERY_UPDATE,
    QUERY_DELETE,
    QUERY_COLUMN,
    QUERY_SET,
    QUERY_FROM,
    QUERY_WHERE,
    QUERY_COND_WHERE,
    QUERY_AND,
    QUERY_OR,
    QUERY_ORDERBY,
    QUERY_GROUPBY,
    QUERY_EXECUTED,
    QUERY_PREPARED,
    QUERY_BOUND
  };

public:
  /**
   * Create a new query for the
   * given session.
   * 
   * @param s The session.
   */
  query(session &s);
  
  /**
   * Create a new query for the
   * given database.
   * 
   * @param s The database.
   */
  query(database &s);
  ~query();

  /**
   * Creates a create statement based
   * on the given prototype node.
   * 
   * @param node The prototype node used for the create statement.
   * @return A reference to the query.
   */
  query& create(const prototype_node &node);

  /**
   * Creates a create statement based
   * on a given name and a serializable
   * class instance.
   * 
   * @param name The name of the table to create.
   * @param o The serializable object providing the field information.
   * @return A reference to the query.
   */
  query& create(const std::string &name, serializable *o);

  /**
   * Creates a drop statement based
   * on the given prototype node.
   * 
   * @param node The prototype node used for the drop statement.
   * @return A reference to the query.
   */
  query& drop(const prototype_node &node);

  /**
   * Creates a drop statement based
   * on the given table name.
   * 
   * @param name The name of the table.
   * @return A reference to the query.
   */
  query& drop(const std::string &name);

  /**
   * Creates a select statement without
   * any selection. All columns must be
   * added manually via the column method.
   * 
   * @return A reference to the query.
   */
  query& select();

  /**
   * Creates a select statement based
   * on the given prototype node.
   * 
   * @param node The prototype node used for the select statement.
   * @return A reference to the query.
   */
  query& select(const prototype_node &node);

  /**
   * Creates a select statement based
   * on the given serializable object.
   * 
   * @param o The serializable object used for the select statement.
   * @return A reference to the query.
   */
  query& select(serializable *o);

  /**
   * Creates an insert statement based
   * on the given object.
   * 
   * @param o The object used for the insert statement.
   * @return A reference to the query.
   */
  query& insert(object_base_ptr &optr);
  query& insert(object_proxy *proxy);

  /**
   * Creates an insert statement based
   * on the given serializable object and.
   * the name of the table
   * 
   * @param o The serializable object used for the insert statement.
   * @param name The name of the table.
   * @return A reference to the query.
   */
  query& insert(serializable *o, const std::string &name);

  /**
   * Creates an update statement based
   * on the given object.
   * 
   * @param o The object used for the update statement.
   * @return A reference to the query.
   */
  query& update(object_base_ptr &optr);
  query& update(object_proxy *proxy);

  /**
   * Creates an update statement based
   * on the given serializable object and.
   * the name of the table
   * 
   * @param name The name of the table.
   * @param o The serializable object used for the update statement.
   * @return A reference to the query.
   */
  query& update(const std::string &name, serializable *o);

  /**
   * Creates an update statement without
   * any settings. All columns must be
   * added manually via the set method. Just
   * the name of the table is given.
   * 
   * @param table The name of the table.
   * @return A reference to the query.
   */
  query& update(const std::string &table);

  /**
   * Creates a delete statement based
   * on the given prototype node.
   * 
   * @param node The prototype node used for the delete statement.
   * @return A reference to the query.
   */
  query& remove(const prototype_node &node);

  /**
   * Adds a where clause string to the select or
   * update statement. For any other query an
   * exception is thrown.
   * 
   * @param clause The where clause.
   * @return A reference to the query.
   */
  query& where(const std::string &clause);

  /**
   * Adds a where clause condition to the select or
   * update statement. For any other query an
   * exception is thrown.
   * 
   * @param c The condition.
   * @return A reference to the query.
   */
  query& where(const condition &c);

  /**
   * Adds an and clause condition to the where
   * clause.
   * 
   * @param c The condition.
   * @return A reference to the query.
   */
  query& and_(const condition &c);
  
  /**
   * Adds an or clause condition to the where
   * clause.
   * 
   * @param c The condition.
   * @return A reference to the query.
   */
  query& or_(const condition &c);

  /**
   * Adds an order by clause to a select
   * statement.
   * 
   * @param by The order by clause.
   * @return A reference to the query.
   */
  query& order_by(const std::string &by);

  /**
   * Adds a limit clause to a select
   * statement.
   * 
   * @param l The limit clause.
   * @return A reference to the query.
   */
  query& limit(int l);

  /**
   * Adds a group by clause to a select
   * statement.
   * 
   * @param field The group by clause.
   * @return A reference to the query.
   */
  query& group_by(const std::string &field);

  /**
   * Adds a column to a select statement.
   * 
   * @param name The name of the column.
   * @param type The datatype of the column,
   * @return A reference to the query.
   */
  query& column(const std::string &name, data_type_t type);
  
  /**
   * Appends the given table name to the
   * select query. Must only be called for a
   * select statement and after the queried
   * columns are appended.
   * 
   * @param table The name of the table.
   * @return A reference to the query.
   */
  query& from(const std::string &table);

  /**
   * This method must only be called for
   * an update statement. Sets for the given
   * column the data type and a value.
   * 
   * @tparam T The value type.
   * @param column The column name.
   * @param type The data type.
   * @param val The value to set.
   * @return A reference to the query.
   */
  template < class T >
  query& set(const std::string &column, data_type_t type, const T &val)
  {
    throw_invalid(QUERY_SET, state);

    if (state == QUERY_SET) {
      sql_.append(", ");
    }

    sql_.append(column + "=");
    std::stringstream valstr;
    valstr << val;
    sql_.append(column.c_str(), type, valstr.str());

    state = QUERY_SET;

    return *this;
  }

  /**
   * Executes the current query and
   * returns a new result object.
   * 
   * @return The result object.
   */
  result* execute();
  
  /**
   * Creates and returns a prepared
   * statement based on the current query.
   * 
   * @return The new prepared statement.
   */
  statement* prepare();

  /**
   * Resets the query.
   * 
   * @return A reference to the query.
   */
  query& reset();
  
private:
  void throw_invalid(state_t next, state_t current) const;

private:
  sql sql_;
  state_t state;
  database &db_;
#ifdef _MSC_VER
  std::auto_ptr<statement> stmt;
#else
  std::unique_ptr<statement> stmt;
#endif
};

}

#endif /* QUERY_HPP */
