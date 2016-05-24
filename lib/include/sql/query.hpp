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
#include "sql/dialect_token.hpp"
#include "sql/column.hpp"
#include "sql/result.hpp"
#include "sql/statement.hpp"
#include "sql/connection.hpp"
#include "sql/typed_column_serializer.hpp"
#include "sql/column_serializer.hpp"
#include "sql/value_serializer.hpp"
#include "sql/value_column_serializer.hpp"
#include "sql/row.hpp"

#include <memory>
#include <sstream>

namespace oos {

namespace detail {

class basic_query
{
public:
  basic_query(const std::string &table_name)
      : state(QUERY_BEGIN)
      , table_name_(table_name)
      , update_columns_(new columns(columns::WITHOUT_BRACKETS))
  {}

  /**
   * Resets the query.
   *
   * @return A reference to the query.
   */
  void reset_query(t_query_command query_command)
  {
    sql_.reset(query_command);
    state = QUERY_BEGIN;
    update_columns_->columns_.clear();
  }

protected:
  enum state_t {
    QUERY_BEGIN,
    QUERY_CREATE,
    QUERY_DROP,
    QUERY_SELECT,
    QUERY_INSERT,
    QUERY_UPDATE,
    QUERY_DELETE,
    QUERY_COLUMN,
    QUERY_SET,
    QUERY_FROM,
    QUERY_WHERE,
    QUERY_COND_WHERE,
    QUERY_ORDERBY,
    QUERY_ORDER_DIRECTION,
    QUERY_GROUPBY,
    QUERY_EXECUTED,
    QUERY_PREPARED,
    QUERY_BOUND
  };

protected:
  static void throw_invalid(state_t next, state_t current)
  {
    std::stringstream msg;
    switch (next) {
      case basic_query::QUERY_CREATE:
      case basic_query::QUERY_DROP:
      case basic_query::QUERY_SELECT:
      case basic_query::QUERY_INSERT:
      case basic_query::QUERY_UPDATE:
      case basic_query::QUERY_DELETE:
        if (current != QUERY_BEGIN) {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      case basic_query::QUERY_WHERE:
      case basic_query::QUERY_COND_WHERE:
        if (current != basic_query::QUERY_SELECT &&
            current != basic_query::QUERY_COLUMN &&
            current != basic_query::QUERY_UPDATE &&
            current != basic_query::QUERY_SET &&
            current != basic_query::QUERY_DELETE)
        {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      case basic_query::QUERY_COLUMN:
        if (current != basic_query::QUERY_SELECT &&
            current != basic_query::QUERY_COLUMN)
        {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      case basic_query::QUERY_FROM:
        if (current != basic_query::QUERY_COLUMN) {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      case basic_query::QUERY_SET:
        if (current != basic_query::QUERY_UPDATE &&
            current != basic_query::QUERY_SET)
        {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      case basic_query::QUERY_ORDERBY:
        if (current != basic_query::QUERY_SELECT &&
            current != basic_query::QUERY_WHERE &&
            current != basic_query::QUERY_COND_WHERE)
        {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      case QUERY_ORDER_DIRECTION:
        if (current != basic_query::QUERY_ORDERBY) {
          msg << "invalid next state: [" << next << "] (current: " << current << ")";
          throw std::logic_error(msg.str());
        }
        break;
      default:
        throw std::logic_error("unknown state");
    }
  }

protected:
  sql sql_;
  state_t state;
  std::string table_name_;
  std::shared_ptr<columns> update_columns_;
};

}
//class condition;

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
template < class T = row >
class query : public detail::basic_query
{
public:
  /**
   * Create a new query for the
   * given connection.
   * 
   * @param conn The connection.
   */
  query(const std::string &table_name)
    : basic_query(table_name)
  {}
  
  ~query() {}

  /**
   * Creates a create statement based
   * on a given name.
   * 
   * @param name The name of the table to create.
   * @return A reference to the query.
   */
  query& create()
  {
    return create(&obj_);
  }

  /**
   * Creates a create statement based
   * on a given name and a serializable
   * class instance.
   *
   * @param name The name of the table to create.
   * @param obj The serializable providing the column information.
   * @return A reference to the query.
   */
  query& create(T *obj)
  {
    reset(t_query_command::CREATE);

    sql_.append(new detail::create(table_name_));

    detail::typed_column_serializer serializer(sql_);

    std::unique_ptr<columns> cols(serializer.execute(*obj));

    sql_.append(cols.release());

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
  query& drop()
  {
    reset(t_query_command::DROP);
    sql_.append(new detail::drop(table_name_));

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
    reset(t_query_command::SELECT);

    throw_invalid(QUERY_SELECT, state);
    sql_.append(new detail::select);

    detail::column_serializer serializer(sql_, columns::WITHOUT_BRACKETS);

    std::unique_ptr<columns> cols(serializer.execute(obj_));

    sql_.append(cols.release());

    sql_.append(new detail::from(table_name_));

    state = QUERY_SELECT;
    return *this;
  }

  /**
   * Creates an insert statement
   *
   * @return A reference to the query.
   */
  query& insert()
  {
    return insert(&obj_);
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
  query& insert(T *obj)
  {
    reset(t_query_command::INSERT);

    sql_.append(new detail::insert(table_name_));

    detail::column_serializer serializer(sql_, columns::WITH_BRACKETS);

    std::unique_ptr<columns> cols(serializer.execute(*obj));

    sql_.append(cols.release());

    detail::value_serializer vserializer(sql_);

    std::unique_ptr<detail::values> vals(vserializer.execute(*obj));

    sql_.append(vals.release());

    state = QUERY_INSERT;

    return *this;
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
  query& update()
  {
    reset(t_query_command::UPDATE);

    sql_.append(new detail::update(table_name_));
    sql_.append(new detail::set);
    sql_.append(update_columns_);

    state = QUERY_UPDATE;
    return *this;
  }

  query& from(const std::string &table)
  {
    sql_.append(new detail::from(table));

    return *this;
  }
  /**
   * This method must only be called for
   * an update statement. Sets for all object
   * attributes column and values of the internal
   * object. To call this methods makes only sense
   * if the query will be prepared afterwards.
   *
   * @return A reference to the query.
   */
  query& set()
  {
    return set(&obj_);
  }

  /**
   * This method must only be called for
   * an update statement. Sets for all object
   * attributes column and values.
   *
   * @tparam T The object type.
   * @param obj The object.
   * @return A reference to the query.
   */
  query& set(T *obj)
  {
    throw_invalid(QUERY_SET, state);

    detail::value_column_serializer serializer(sql_);

    // Todo: pass member update_columns_
    serializer.append_to(update_columns_, *obj);

//    sql_.append(cols.release());

    state = QUERY_SET;

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
//  query& set(const std::string &column, const V &val)
  query& set(const std::string &column, V &val)
  {
    throw_invalid(QUERY_SET, state);

    update_columns_->columns_.push_back(std::make_shared<detail::value_column<V>>(column, val));

    state = QUERY_SET;

    return *this;
  }


  /**
   * Creates a delete statement based
   * on the given prototype node.
   * 
   * @param table The table name to remove from.
   * @return A reference to the query.
   */
  query& remove()
  {
    reset(t_query_command::DELETE);

    sql_.append(new detail::remove(table_name_));

    state = QUERY_DELETE;

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
  template < class COND >
  query& where(const COND &c)
  {
    throw_invalid(QUERY_COND_WHERE, state);

    sql_.append(new detail::where(c));
//    sql_.append(new condition<COND>(c));

    state = QUERY_COND_WHERE;
    return *this;
  }

  /**
   * Adds an order by clause to a select
   * statement.
   * 
   * @param by The order by clause.
   * @return A reference to the query.
   */
  query& order_by(const std::string &column)
  {
    throw_invalid(QUERY_ORDERBY, state);

    sql_.append(new detail::order_by(column));

    state = QUERY_ORDERBY;

    return *this;
  }

  /**
   * Order result of select query ascending.
   *
   * @return A reference to the query.
   */
  query& asc()
  {
    throw_invalid(QUERY_ORDER_DIRECTION, state);

    sql_.append(new detail::asc);

    state = QUERY_ORDER_DIRECTION;

    return *this;
  }

  /**
   * Order result of select query descending.
   *
   * @return A reference to the query.
   */
  query& desc()
  {
    throw_invalid(QUERY_ORDER_DIRECTION, state);

    sql_.append(new detail::desc);

    state = QUERY_ORDER_DIRECTION;

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
    sql_.append(new detail::top(l));
    return *this;
  }

  /**
   * Adds a group by clause to a select
   * statement.
   * 
   * @param column The group by clause.
   * @return A reference to the query.
   */
  query& group_by(const std::string &col)
  {
    throw_invalid(QUERY_GROUPBY, state);

    sql_.append(new detail::group_by(col));

    state = QUERY_GROUPBY;

    return *this;
  }

  /**
   * Adds a column to a select statement.
   * 
   * @param column The name of the column.
   * @param type The datatype of the column,
   * @return A reference to the query.
   */
  query& column(const std::string &field, data_type_t type, size_t index)
  {
    throw_invalid(QUERY_COLUMN, state);
    sql_.append(new detail::typed_column(field, type, index, false));
    state = QUERY_COLUMN;
    return *this;
  }

  query& reset(t_query_command query_command)
  {
    reset_query(query_command);
    return *this;
  }
  /**
   * Executes the current query and
   * returns a new result serializable.
   * 
   * @return The result serializable.
   */
  result<T> execute(connection &conn)
  {
//    std::cout << "SQL: " << sql_.direct().c_str() << '\n';
//    std::cout.flush();
    return conn.execute<T>(sql_);
  }

  /**
   * Creates and returns a prepared
   * statement based on the current query.
   * 
   * @return The new prepared statement.
   */
  statement<T> prepare(connection &conn)
  {
    return conn.prepare<T>(sql_);
  }

private:
  T obj_;
};

template <>
class query<row> : public detail::basic_query
{
public:
  query()
    : basic_query("")
  {}

  /**
   * Create a new query for the
   * given connection.
   *
   * @param conn The connection.
   */
  query(const std::string &table_name)
    : basic_query(table_name)
  {}

  ~query() {}

  /**
   * Creates a select statement with the
   * given columns.
   *
   * @param cols The columns to select
   * @return A reference to the query.
   */
  query& select(columns cols)
  {
    reset(t_query_command::SELECT);

    throw_invalid(QUERY_SELECT, state);
    sql_.append(new detail::select);

    cols.without_brackets();
    sql_.append(new columns(cols));

    for (auto &&column : cols.columns_) {
      row_.add_column(column->name);
    }

    state = QUERY_SELECT;
    return *this;
  }

  query& from(const std::string &table)
  {
    sql_.append(new detail::from(table));

    table_name_ = table;

    return *this;
  }

  template < class COND >
  query& where(const COND &c)
  {
    throw_invalid(QUERY_COND_WHERE, state);

    sql_.append(new detail::where(c));

    state = QUERY_COND_WHERE;
    return *this;
  }

  query& reset(t_query_command query_command)
  {
    reset_query(query_command);
    return *this;
  }

  result<row> execute(connection &conn)
  {
//    std::cout << "SQL: " << sql_.direct().c_str() << '\n';
//    std::cout.flush();
    return conn.execute<row>(sql_, table_name_, row_);
  }

  statement<row> prepare(connection &conn)
  {
//    std::cout << "SQL: " << sql_.prepare().c_str() << '\n';
//    std::cout.flush();
    return conn.prepare<row>(sql_, table_name_, row_);
  }

  std::string str(connection &conn, bool prepared)
  {
    if (prepared) {
      return conn.dialect()->prepare(sql_);
    } else {
      return conn.dialect()->direct(sql_);
    }
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
    sql_.append(new detail::top(l));
    return *this;
  }

private:
  row row_;
};

//query<row> select(columns cols);
//
//query<row> select(columns cols)
//{
//  query<row> q;
//  return q.select(cols);
//}

}

#endif /* QUERY_HPP */
