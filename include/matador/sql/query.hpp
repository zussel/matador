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
#ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_SQL_API
#endif

#include "matador/sql/basic_query.hpp"
#include "matador/sql/dialect_token.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/result.hpp"
#include "matador/sql/statement.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/typed_column_serializer.hpp"
#include "matador/sql/column_serializer.hpp"
#include "matador/sql/value_serializer.hpp"
#include "matador/sql/value_column_serializer.hpp"
#include "matador/sql/row.hpp"

#include "matador/utils/any.hpp"
#include "matador/utils/any_visitor.hpp"

#include <memory>
#include <sstream>

namespace matador {

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
   * @brief Create a new query.
   *
   * Create a new query with unset
   * default table name and unset
   * default internal connection
   */
  query()
    : basic_query("")
  {
    determine_tablename<T>();
  }

  /**
   * @brief Create a new query for the given table.
   *
   * @param table_name The default table name used for the query.
   */
  explicit query(const std::string &table_name)
    : basic_query(table_name)
  {}

  ~query() = default;

  /**
   * Bind a table name to a specific
   * object type
   *
   * @param table_name The tablename to bind
   */
  static void bind_table(const std::string &table_name)
  {
    tablename_map_.insert(std::make_pair(std::type_index(typeid(T)), table_name));
  }

  /**
   * Remove all bound tablenames
   */
  static void clear_bound_tables()
  {
    tablename_map_.clear();
  }

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
    sql_.table_name(table_name_);

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
    sql_.table_name(table_name_);

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
      cols->push_back(std::make_shared<matador::column>(col));
    }

    sql_.append(cols.release());

    sql_.append(new detail::from(table_name_));
    sql_.table_name(table_name_);

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
    sql_.table_name(table_name_);

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
    sql_.table_name(table_name_);
    sql_.append(new detail::set);
    sql_.append(update_columns_);

    state = QUERY_UPDATE;

    detail::value_column_serializer vcserializer;

    vcserializer.append_to(update_columns_, obj);

    state = QUERY_SET;

    return *this;
  }

  /**
   * Creates an update statement without
   * any settings. Sets for all column value pairs
   * attributes column and values.
   *
   * @param colvalues The column name and value list
   * @return A reference to the query.
   */
  query& update(const std::initializer_list<std::pair<std::string, matador::any>> &colvalues)
  {
    reset(t_query_command::UPDATE);

    sql_.append(new detail::update);
    sql_.append(new detail::tablename(table_name_));
    sql_.table_name(table_name_);
    sql_.append(new detail::set);

    for (auto colvalue : colvalues) {
      query_value_column_processor_.execute(colvalue);
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
    sql_.table_name(table_name_);

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
    sql_.table_name(table_name_);

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

/**
 * @brief A row based query
 *
 * This query can be used for generic row
 * based queries where now concrete type exists.
 */
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
    : basic_query("")
  {}

  /**
   * @brief Create a new query for the
   * given table.
   *
   * @param table_name The default table name used for the query.
   */
  explicit query(const std::string &table_name)
    : basic_query(table_name)
  {}

  ~query() = default;

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
   * @param table_name The table name to be used for the statement
   * @param collist The columns to be created
   * @return A reference to the query.
   */
  query& create(const std::string &table_name, const std::initializer_list<std::shared_ptr<detail::typed_column>> &collist)
  {
    reset(t_query_command::CREATE);

    sql_.append(new detail::create(table_name));
    sql_.table_name(table_name_);

    std::unique_ptr<matador::columns> cols(new matador::columns(matador::columns::WITH_BRACKETS));
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
   * @brief Start a drop query for the given table name
   * @param table_name The table to be dropped
   * @return A reference to the query.
   */
  query& drop(const std::string &table_name)
  {
    reset(t_query_command::DROP);
    sql_.append(new detail::drop(table_name));
    sql_.table_name(table_name_);

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
    sql_.table_name(table_name_);

    std::unique_ptr<columns> cols(new matador::columns);

    for (const auto& name : column_names) {
      cols->push_back(std::make_shared<matador::column>(name));
    }

    sql_.append(cols.release());

    state = QUERY_INSERT;

    return *this;
  }

  /**
   * Initializes the values of an insert statement
   * from the given initializer_list. The list consists
   * of any objects representing the values.
   * 
   * @param values The values to set.
   * @return A reference to the query.
   */
  query& values(const std::initializer_list<matador::any> &values)
  {
    std::unique_ptr<detail::values> vals(new detail::values);

    // append values
    for (auto value : values) {
      vals->push_back(query_value_creator_.create_from_any(value));
    }
    sql_.append(vals.release());

    return *this;
  }

  /**
   * Creates an update statement without
   * any settings. Sets for all column value pairs
   * attributes column and values.
   *
   * @param colvalues The column name and value list
   * @return A reference to the query.
   */
  query& update(const std::initializer_list<std::pair<std::string, matador::any>> &colvalues)
  {
    reset(t_query_command::UPDATE);

    sql_.append(new detail::update);
    sql_.append(new detail::tablename(table_name_));
    sql_.table_name(table_name_);
    sql_.append(new detail::set);

    for (auto colvalue : colvalues) {
      query_value_column_processor_.execute(colvalue);
    }

    sql_.append(update_columns_);

    state = QUERY_SET;
    return *this;
  }

  /**
   * Appends all columns from initializer_list
   * to a select statement.
   * 
   * @param colnames The column list to select
   * @return A reference to the query.
   */
  query& select(const std::initializer_list<std::string> &colnames)
  {
    reset(t_query_command::SELECT);

    throw_invalid(QUERY_SELECT, state);
    sql_.append(new detail::select);

    std::unique_ptr<matador::columns> cols(new matador::columns(colnames, matador::columns::WITHOUT_BRACKETS));

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
  query& select(matador::columns cols)
  {
    reset(t_query_command::SELECT);

    throw_invalid(QUERY_SELECT, state);
    sql_.append(new detail::select);

    cols.without_brackets();
    sql_.append(new matador::columns(cols));

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
    sql_.table_name(table_name_);

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
   * @brief Execute the query.
   *
   * Execute the query for the given connection
   *
   * @param conn The connection used by the query.
   * @return The result of the query
   */
  result<row> execute(connection &conn)
  {
    return conn.execute<row>(sql_, table_name_, row_);
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

private:
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
OOS_SQL_API query<row> select(columns cols);

}

#endif /* QUERY_HPP */
