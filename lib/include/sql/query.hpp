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

#include "sql/basic_query.hpp"
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

#include "tools/any.hpp"

#include <memory>
#include <sstream>

namespace oos {

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
   * table.
   *
   * @param table_name The name of the table
   */
  query(const std::string &table_name)
    : basic_query(table_name)
  {}
  
  ~query() {}

  /**
   * Creates a create statement.
   * 
   * @return A reference to the query.
   */
  query& create()
  {
    return create(obj_);
  }

  /**
   * Creates a create statement based
   * on a given object pointer
   *
   * @param obj The serializable providing the column information.
   * @return A reference to the query.
   */
  query& create(T obj)
  {
    reset(t_query_command::CREATE);

    sql_.append(new detail::create(table_name_));

    detail::typed_column_serializer serializer;

    std::unique_ptr<columns> cols(serializer.execute(obj));

    sql_.append(cols.release());

    state = QUERY_CREATE;
    return *this;
  }

  /**
   * Creates a drop statement.
   * 
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
   * Creates a select statement.
   * 
   * @return A reference to the query.
   */
  query& select()
  {
    reset(t_query_command::SELECT);

    throw_invalid(QUERY_SELECT, state);
    sql_.append(new detail::select);

    detail::column_serializer serializer(columns::WITHOUT_BRACKETS);

    std::unique_ptr<columns> cols(serializer.execute(obj_));

    sql_.append(cols.release());

    sql_.append(new detail::from(table_name_));

    state = QUERY_FROM;
    return *this;
  }

  /**
   * Creates a select statement for the given columns.
   *
   * @param column_names A list of column names to select
   * @return A reference to the query.
   */
  query& select(const std::initializer_list<std::string> &column_names)
  {
    reset(t_query_command::SELECT);

    throw_invalid(QUERY_SELECT, state);
    sql_.append(new detail::select);

    std::unique_ptr<columns> cols(new columns(columns::WITHOUT_BRACKETS));
    for (auto &&col : column_names) {
      cols->push_back(std::make_shared<oos::column>(col));
    }

    sql_.append(cols.release());

    sql_.append(new detail::from(table_name_));

    state = QUERY_FROM;
    return *this;
  }

  /**
   * Creates an insert statement
   *
   * @return A reference to the query.
   */
  query& insert()
  {
    return insert(obj_);
  }

  /**
   * Creates an insert statement based
   * on the given object.
   *
   * @param obj The serializable used for the insert statement.
   * @return A reference to the query.
   */
  query& insert(T &obj)
  {
    reset(t_query_command::INSERT);

    sql_.append(new detail::insert(table_name_));

    detail::column_serializer serializer(columns::WITH_BRACKETS);

    std::unique_ptr<columns> cols(serializer.execute(obj));

    sql_.append(cols.release());

    detail::value_serializer vserializer;

    std::unique_ptr<detail::values> vals(vserializer.execute(obj));

    sql_.append(vals.release());

    state = QUERY_INSERT;

    return *this;
  }

  /**
   * Creates an update statement without
   * any settings. Sets for all object
   * attributes column and values of the internal
   * object. To call this methods makes only sense
   * if the query will be prepared afterwards.
   * 
   * @return A reference to the query.
   */
  query& update()
  {
    return update(obj_);
  }

  /**
   * Creates an update statement without
   * any settings. Sets for all object
   * attributes column and values.
   *
   * @param obj The object to be updated.
   * @return A reference to the query.
   */
  query& update(T &obj)
  {
    reset(t_query_command::UPDATE);

    sql_.append(new detail::update);
    sql_.append(new detail::tablename(table_name_));
    sql_.append(new detail::set);
    sql_.append(update_columns_);

    state = QUERY_UPDATE;

    detail::value_column_serializer vcserializer;

    vcserializer.append_to(update_columns_, obj);

    state = QUERY_SET;

    return *this;
  }

  query& update(const std::initializer_list<std::pair<std::string, oos::any>> &colvalues)
  {
    reset(t_query_command::UPDATE);

    sql_.append(new detail::update);
    sql_.append(new detail::tablename(table_name_));
    sql_.append(new detail::set);

    for (auto &&colvalue : colvalues) {
      rowvalues_.push_back(colvalue.second);
      if (colvalue.second.is<int>()) {
        std::shared_ptr<detail::value_column<int>> ival(new detail::value_column<int>(colvalue.first, rowvalues_.back()._<int>()));
        update_columns_->push_back(ival);
      } else if (colvalue.second.is<unsigned long>()) {
        std::shared_ptr<detail::value_column<unsigned long>> ival(new detail::value_column<unsigned long>(colvalue.first, rowvalues_.back()._<unsigned long>()));
        update_columns_->push_back(ival);
      } else if (colvalue.second.is<const char*>()) {
        update_columns_->push_back(std::make_shared<detail::value_column<const char*>>(
          colvalue.first,
          rowvalues_.back()._<const char*>(),
          strlen(rowvalues_.back()._<const char*>())
        ));
      }
    }

    sql_.append(update_columns_);

    state = QUERY_SET;
    return *this;
  }

  /**
   * @brief Specfies the from token of a query
   *
   * Specfies the from token of a query with
   * a table name as argument.
   *
   * @param table The name of the table
   * @return A reference to the query.
   */
  query& from(const std::string &table)
  {
    sql_.append(new detail::from(table));

    return *this;
  }

  /**
   * Creates a delete statement.
   * 
   * @return A reference to the query.
   */
  query& remove()
  {
    reset(t_query_command::REMOVE);

    sql_.append(new detail::remove());
    sql_.append(new detail::from(table_name_));

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
   * @param col The column for the order by clause
   * @return A reference to the query.
   */
  query& order_by(const std::string &col)
  {
    throw_invalid(QUERY_ORDERBY, state);

    sql_.append(new detail::order_by(col));

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
   * @param col The group by clause.
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
   * @param type The datatype of the column.
   * @param index The index of the column.
   * @return A reference to the query.
   */
  query& column(const std::string &column, data_type type, size_t index)
  {
    throw_invalid(QUERY_COLUMN, state);
    sql_.append(new detail::typed_column(column, type, index, false));
    state = QUERY_COLUMN;
    return *this;
  }

  /**
   * @brief Resets the query.
   *
   * @param query_command The query command to which the query is resetted
   * @return A reference to the query.
   */
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
   * @param conn The connection.
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
  /**
   * @brief Create a new query.
   *
   * Create a new query with unset
   * default tablename and unset
   * default internal connection
   */
  query()
    : query(nullptr, "")
  {}

  /**
   * @brief Create a new query for the
   * given table.
   *
   * @param tablename The default tablename used for the query.
   */
  query(const std::string &tablename)
    : query(nullptr, tablename)
  {}

  /**
   * @brief Create a query with a default connection
   *
   * @param conn The default connection to be used
   */
  query(connection *conn)
    : query(conn, "")
  {}

  /**
   * @brief Create a query with a default connection and default tablename
   *
   * @param conn The default connection to be used
   * @param tablename The default tablename used for the query.
   */
  query(connection *conn, const std::string &tablename)
    : detail::basic_query(tablename)
    , connection_(conn)
  {}

  ~query() {}

  /**
   * @brief Start a create query for default table
   * @param collist The columns to be created
   * @return A reference to the query.
   */
  query& create(const std::initializer_list<std::shared_ptr<detail::typed_column>> &collist)
  {
    return create(table_name_, collist);
  }

  /**
   * @brief Create a table with given name
   *
   * @param tablename The tablename to be used for the statement
   * @param collist The columns to be created
   * @return A reference to the query.
   */
  query& create(const std::string &tablename, const std::initializer_list<std::shared_ptr<detail::typed_column>> &collist)
  {
    reset(t_query_command::CREATE);

    sql_.append(new detail::create(tablename));

    std::unique_ptr<oos::columns> cols(new oos::columns(oos::columns::WITH_BRACKETS));
    for (auto &&col : collist) {
      cols->push_back(col);
    }

    sql_.append(cols.release());

    state = QUERY_CREATE;
    return *this;
  }

  /**
   * @brief Start a drop query for the default tablename
   * @return A reference to the query.
   */
  query& drop()
  {
    return drop(table_name_);
  }

  /**
   * @brief Start a drop query for the given tablename
   * @param tablename The table to be dropped
   * @return A reference to the query.
   */
  query& drop(const std::string &tablename)
  {
    reset(t_query_command::DROP);
    sql_.append(new detail::drop(tablename));

    state = QUERY_DROP;
    return *this;
  }

  /**
   * @brief Create an insert statement for given columns.
   * @param column_names List of column to insert
   * @return A reference to the query.
   */
  query& insert(const std::initializer_list<std::string> &column_names)
  {
    reset(t_query_command::INSERT);

    sql_.append(new detail::insert(table_name_));

    std::unique_ptr<columns> cols(new oos::columns);

    for (auto name : column_names) {
      cols->push_back(std::make_shared<oos::column>(name));
    }

    sql_.append(cols.release());

    state = QUERY_INSERT;

    return *this;
  }

  query& values(const std::initializer_list<oos::any> &values)
  {
    std::unique_ptr<detail::values> vals(new detail::values);

    // append values
    for (auto value : values) {
      if (value.is<int>()) {
        vals->push_back(std::make_shared<oos::value<int>>(value._<int>()));
      } else if (value.is<const char*>()) {
        vals->push_back(std::make_shared<oos::value<std::string>>(value._<const char*>()));
      }
    }
    sql_.append(vals.release());

    return *this;
  }

  query& update(const std::initializer_list<std::pair<std::string, oos::any>> &colvalues)
  {
    reset(t_query_command::UPDATE);

    sql_.append(new detail::update);
    sql_.append(new detail::tablename(table_name_));
    sql_.append(new detail::set);

    for (auto &&colvalue : colvalues) {
      rowvalues_.push_back(colvalue.second);
      if (colvalue.second.is<int>()) {
        std::shared_ptr<detail::value_column<int>> ival(new detail::value_column<int>(colvalue.first, rowvalues_.back()._<int>()));
        update_columns_->push_back(ival);
      } else if (colvalue.second.is<const char*>()) {
        update_columns_->push_back(std::make_shared<detail::value_column<const char*>>(
          colvalue.first,
          rowvalues_.back()._<const char*>(),
          strlen(rowvalues_.back()._<const char*>())
        ));
      }
    }

    sql_.append(update_columns_);

    state = QUERY_SET;
    return *this;
  }

  query& select(const std::initializer_list<std::string> &colnames)
  {
    reset(t_query_command::SELECT);

    throw_invalid(QUERY_SELECT, state);
    sql_.append(new detail::select);

    std::unique_ptr<oos::columns> cols(new oos::columns(colnames, oos::columns::WITHOUT_BRACKETS));

    for (auto &&column : cols->columns_) {
      row_.add_column(column->name);
    }

    sql_.append(cols.release());

    state = QUERY_SELECT;
    return *this;
  }
  /**
   * @brief Creates a select statement with the
   * given columns.
   *
   * @param cols The columns to select
   * @return A reference to the query.
   */
  query& select(oos::columns cols)
  {
    reset(t_query_command::SELECT);

    throw_invalid(QUERY_SELECT, state);
    sql_.append(new detail::select);

    cols.without_brackets();
    sql_.append(new oos::columns(cols));

    for (auto &&column : cols.columns_) {
      row_.add_column(column->name);
    }

    state = QUERY_SELECT;
    return *this;
  }

  /**
   * @brief Specfies the from token of a query
   *
   * Specfies the from token of a query with
   * a table name as argument.
   *
   * @param table The name of the table
   * @return A reference to the query.
   */
  query& from(const std::string &table)
  {
    throw_invalid(QUERY_FROM, state);

    sql_.append(new detail::from(table));

    table_name_ = table;

    return *this;
  }

  /**
   * @brief Specfies the from token of a query
   *
   * Specfies the from token of a query with
   * a sub query as argument.
   *
   * @param q The sub query
   * @return A reference to the query.
   */
  query& from(detail::basic_query &q)
  {
    throw_invalid(QUERY_FROM, state);

    sql_.append(new detail::from(""));
    sql_.append(q.stmt());

    return *this;
  }

  /**
   * @brief Adds a where clause condition to the query.
   *
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

    state = QUERY_COND_WHERE;
    return *this;
  }

  /**
   * @brief Resets the query.
   *
   * @param query_command The query command to which the query is resetted
   * @return A reference to the query.
   */
  query& reset(t_query_command query_command)
  {
    reset_query(query_command);
    row_.clear();
    rowvalues_.clear();
    return *this;
  }

  /**
   * @brief Execute the query
   *
   * Execute the query with the stored
   * connection. If internal connection is null
   * an exception is thrown.
   *
   * @throws std::logic_error
   * @return The result of the query
   */
  result<row> execute()
  {
    if (connection_ == nullptr) {
      throw std::logic_error("connection is nullptr");
    }
    return execute(*connection_);
  }
  /**
   * @brief Execute the query.
   *
   * Execute the query for the given connection
   *
   * @param conn The connection used by the query.
   * @return The result of the query
   */
  result<row> execute(connection &conn)
  {
//    std::cout << "SQL: " << conn.dialect()->direct(sql_) << '\n';
//    std::cout.flush();
    return conn.execute<row>(sql_, table_name_, row_);
  }

  /**
   * @brief Prepare the query
   *
   * Prepare the query with the stored
   * connection. If internal connection is null
   * an exception is thrown.
   *
   * @throws std::logic_error
   * @return The prepared statement
   */
  statement<row> prepare()
  {
    if (connection_ == nullptr) {
      throw std::logic_error("connection is nullptr");
    }
    return prepare(*connection_);

  }
  /**
   * @brief Prepares the query.
   *
   * Prepares the query for the given connection
   *
   * @param conn The connection used by the query.
   * @return The prepared statement of the query
   */
  statement<row> prepare(connection &conn)
  {
//    std::cout << "SQL: " << sql_.prepare().c_str() << '\n';
//    std::cout.flush();
    return conn.prepare<row>(sql_, table_name_, row_);
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
   * Specify an alias for the selection
   * or the a table name.
   *
   * @param alias Alias name
   * @return Query object reference
   */
  query& as(const std::string &alias)
  {
    sql_.append(new detail::as(alias));
    return *this;
  }

  std::string str(bool prepared)
  {
    return detail::basic_query::str(*connection_, prepared);
  }

private:
  connection *connection_ = nullptr;
  row row_;
};

/**
 * @brief Creates a select query for type T
 *
 * @tparam T The type of the query.
 * @param table The table name
 * @return A select query for type T.
 */
template < class T >
query<T> select(const std::string &table)
{
  query<T> q(table);
  return q.select();
}

/**
 * @brief Creates a query for given columns
 *
 * @param cols The rows to select
 * @return A select query for anonymous rows.
 */
OOS_API query<row> select(columns cols);

}

#endif /* QUERY_HPP */
