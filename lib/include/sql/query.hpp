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

#include "sql/sql.hpp"
#include "sql/result.hpp"
#include "sql/statement.hpp"
#include "sql/connection.hpp"
#include "sql/query_create.hpp"
#include "sql/query_insert.hpp"
#include "sql/query_update.hpp"
#include "sql/query_select.hpp"

#include <memory>
#include <sstream>

namespace oos {

class condition;

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
 * create them using serializable based class or
 * instances.
 * 
 * The class uses a chain mechanism. Each method
 * representing a part of a query returns the reference
 * to itself. So you can concatenate your query
 * parts by calling the methods in a chain
 * (concatenated by dots).
 */
template < class T >
class query
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
   * given connection.
   * 
   * @param conn The connection.
   */
  query(connection &conn)
    : state(QUERY_BEGIN)
    , connection_(conn)
  {}
  
  ~query() {}

  /**
   * Creates a create statement based
   * on a given name.
   * 
   * @param name The name of the table to create.
   * @return A reference to the query.
   */
  query& create(const std::string &name)
  {
    T obj;
    return create(name, &obj);
  }

  /**
   * Creates a create statement based
   * on a given name and a serializable
   * class instance.
   *
   * @param name The name of the table to create.
   * @param obj The serializable providing the field information.
   * @return A reference to the query.
   */
  query& create(const std::string &name, T *obj)
  {
    reset();
    sql_.append(std::string("CREATE TABLE ") + name + std::string(" ("));

    query_create s(sql_, connection_);
    oos::access::serialize(s, *obj);
//    obj->serialize(s);
    sql_.append(")");

//  std::cout << sql_.str(true) << '\n';

    state = QUERY_CREATE;
    return *this;
  }

  /**
   * Creates a drop statement based
   * on the given table name.
   * 
   * @param name The name of the table.
   * @return A reference to the query.
   */
  query& drop(const std::string &name)
  {
    reset();
    sql_.append(std::string("DROP TABLE ") + name);

    state = QUERY_DROP;
    return *this;
  }

  /**
   * Creates a select statement based
   * on the given serializable serializable.
   * 
   * @return A reference to the query.
   */
  query& select()
  {
    reset();

    throw_invalid(QUERY_SELECT, state);
    sql_.append("SELECT ");

    query_select s(sql_);

    T obj;
    oos::access::serialize(s, obj);

    state = QUERY_SELECT;
    return *this;
  }

  /**
   * Creates an insert statement based
   * on the given serializable and.
   * the name of the table
   *
   * @param obj The serializable used for the insert statement.
   * @param table The name of the table.
   * @return A reference to the query.
   */
  query& insert(T *obj, const std::string &table)
  {
    reset();
    sql_.append(std::string("INSERT INTO ") + table + std::string(" ("));

    query_insert s(sql_);
    s.fields();

    oos::access::serialize(s, *obj);

    sql_.append(") VALUES (");

    s.values();
    oos::access::serialize(s, *obj);

    sql_.append(")");

    state = QUERY_OBJECT_INSERT;

    return *this;
  }

  /**
   * Creates an insert statement based
   * on the given object_ptr and the name of the table
   *
   * @tparam TYPE The flag indicating if the holder represents a reference or a pointer
   * @param holder The object_holder used for the insert statement.
   * @param table The name of the table.
   * @return A reference to the query.
   */
  query& insert(const has_one<T> &holder, const std::string &table)
  {
    return insert(holder.get(), table);
  }

  query& insert(const object_ptr<T> &holder, const std::string &table)
  {
    return insert(holder.get(), table);
  }

  /**
   * Creates an update statement based
   * on the given serializable and.
   * the name of the table
   * 
   * @param o The serializable used for the update statement.
   * @param table The name of the table.
   * @return A reference to the query.
   */
  query& update(T *o, const std::string &table)
  {
    reset();
    sql_.append(std::string("UPDATE ") + table + std::string(" SET "));

    query_update s(sql_);
    oos::access::serialize(s, *o);

    state = QUERY_OBJECT_UPDATE;

    return *this;
  }

  /**
   * Creates an update statement based
   * on the given object_ptr and the name of the table
   *
   * @tparam TYPE The flag indicating if the holder represents a reference or a pointer
   * @param holder The object_holder used for the insert statement.
   * @param table The name of the table.
   * @return A reference to the query.
   */
  template < bool TYPE >
  query& update(has_one<T> &holder, const std::string &table)
  {
    return update(holder.get(), table);
  }

  /**
   * Creates an update statement without
   * any settings. All columns must be
   * added manually via the set method. Just
   * the name of the table is given.
   * 
   * @param table The name of the table.
   * @return A reference to the query.
   */
  query& update(const std::string &table)
  {
    throw_invalid(QUERY_UPDATE, state);
    reset();
    sql_.append("UPDATE " + table + " SET ");
    state = QUERY_UPDATE;
    return *this;
  }

  /**
   * Creates a delete statement based
   * on the given prototype node.
   * 
   * @param table The table name to remove from.
   * @return A reference to the query.
   */
  query& remove(const std::string &table)
  {
    reset();

    sql_.append(std::string("DELETE FROM ") + table);

    state = QUERY_DELETE;

    return *this;
  }
  /**
   * Adds a where clause string to the select or
   * update statement. For any other query an
   * exception is thrown.
   * 
   * @param clause The where clause.
   * @return A reference to the query.
   */
  query& where(const std::string &clause)
  {
    throw_invalid(QUERY_WHERE, state);

    sql_.append(std::string(" WHERE ") + clause);

    state = QUERY_WHERE;

    return *this;
  }

  /**
   * Adds a where clause condition to the select or
   * update statement. For any other query an
   * exception is thrown.
   * 
   * @param c The condition.
   * @return A reference to the query.
   */
  query& where(const condition &c)
  {
    throw_invalid(QUERY_COND_WHERE, state);

    sql_.append(std::string(" WHERE "));
    sql_.append(c);

    state = QUERY_COND_WHERE;
    return *this;
  }

  /**
   * Adds an and clause condition to the where
   * clause.
   * 
   * @param c The condition.
   * @return A reference to the query.
   */
  query& and_(const condition &c)
  {
    throw_invalid(QUERY_AND, state);

    sql_.append(std::string(" AND "));
    sql_.append(c);

    state = QUERY_AND;
    return *this;
  }

  /**
   * Adds an or clause condition to the where
   * clause.
   * 
   * @param c The condition.
   * @return A reference to the query.
   */
  query& or_(const condition &c)
  {
    throw_invalid(QUERY_OR, state);

    sql_.append(std::string(" OR "));
    sql_.append(c);

    state = QUERY_OR;
    return *this;
  }

  /**
   * Adds an order by clause to a select
   * statement.
   * 
   * @param by The order by clause.
   * @return A reference to the query.
   */
  query& order_by(const std::string &by)
  {
    throw_invalid(QUERY_ORDERBY, state);

    sql_.append(std::string(" ORDER BY ") + by);

    state = QUERY_ORDERBY;

    return *this;
  }

  /**
   * Adds a limit clause to a select
   * statement.
   * 
   * @param l The limit clause.
   * @return A reference to the query.
   */
  query& limit(std::size_t l)
  {
    std::stringstream limval;
    limval << " LIMIT(" << l << ")";
    sql_.append(limval.str());
    return *this;
  }

  /**
   * Adds a group by clause to a select
   * statement.
   * 
   * @param field The group by clause.
   * @return A reference to the query.
   */
  query& group_by(const std::string &field)
  {
    throw_invalid(QUERY_GROUPBY, state);

    sql_.append(std::string(" GROUP BY ") + field);

    state = QUERY_GROUPBY;

    return *this;
  }

  /**
   * Adds a column to a select statement.
   * 
   * @param field The name of the column.
   * @param type The datatype of the column,
   * @return A reference to the query.
   */
  query& column(const std::string &field, data_type_t type)
  {
    throw_invalid(QUERY_COLUMN, state);
    if (state == QUERY_COLUMN) {
      sql_.append(", ");
    }
    sql_.append(field.c_str(), type);
    state = QUERY_COLUMN;
    return *this;
  }

  /**
   * Appends the given table name to the
   * select query. Must only be called for a
   * select statement and after the queried
   * columns are appended.
   * 
   * @param table The name of the table.
   * @return A reference to the query.
   */
  query& from(const std::string &table)
  {
    // check state (simple select)
    sql_.append(" FROM " + table);
    return *this;
  }

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
  template < class V >
  query& set(const std::string &column, data_type_t type, const V &val)
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
   * returns a new result serializable.
   * 
   * @return The result serializable.
   */
  result<T> execute()
  {
    std::cout << "SQL: " << sql_.direct().c_str() << '\n';
    std::cout.flush();
    return connection_.execute<T>(sql_.direct());
  }

  /**
   * Creates and returns a prepared
   * statement based on the current query.
   * 
   * @return The new prepared statement.
   */
  statement<T> prepare()
  {
    return connection_.prepare<T>(sql_);
  }

  /**
   * Resets the query.
   * 
   * @return A reference to the query.
   */
  query& reset()
  {
    sql_.reset();
    state = QUERY_BEGIN;
    return *this;
  }


private:
  static void throw_invalid(state_t next, state_t current)
  {
    std::stringstream msg;
    switch (next) {
      case query::QUERY_CREATE:
      case query::QUERY_DROP:
      case query::QUERY_SELECT:
      case query::QUERY_INSERT:
      case query::QUERY_UPDATE:
      case query::QUERY_DELETE:
      case query::QUERY_OBJECT_SELECT:
      case query::QUERY_OBJECT_INSERT:
      case query::QUERY_OBJECT_UPDATE:
        if (current != QUERY_BEGIN) {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      case query::QUERY_WHERE:
      case query::QUERY_COND_WHERE:
        if (current != query::QUERY_SELECT &&
            current != query::QUERY_COLUMN &&
            current != query::QUERY_SET &&
            current != query::QUERY_DELETE &&
            current != query::QUERY_OBJECT_SELECT &&
            current != query::QUERY_OBJECT_UPDATE)
        {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      case query::QUERY_AND:
      case query::QUERY_OR:
        if (current != query::QUERY_COND_WHERE &&
            current != query::QUERY_OR &&
            current != query::QUERY_AND)
        {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      case query::QUERY_COLUMN:
        if (current != query::QUERY_SELECT &&
            current != query::QUERY_COLUMN)
        {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      case query::QUERY_FROM:
        if (current != query::QUERY_COLUMN) {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      case query::QUERY_SET:
        if (current != query::QUERY_UPDATE &&
            current != query::QUERY_SET)
        {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      default:
        throw std::logic_error("unknown state");
    }
  }


private:
  sql sql_;
  state_t state;
  connection &connection_;
};

}

#endif /* QUERY_HPP */
