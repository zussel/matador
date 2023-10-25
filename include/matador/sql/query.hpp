#ifndef QUERY_HPP
#define QUERY_HPP

#include "matador/sql/basic_query.hpp"
#include "matador/sql/dialect_token.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/result.hpp"
#include "matador/sql/statement.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/row.hpp"
#include "matador/sql/column_processor.hpp"

#include "matador/utils/any.hpp"
#include "matador/utils/any_visitor.hpp"

#include <memory>
#include <vector>
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
  query() = default;
  ~query() = default;

  /**
   * Creates a create statement.
   * 
   * @return A reference to the query.
   */
  query& create(const std::string &table_name)
  {
    return create(table_name, obj_);
  }

  /**
   * Creates a create statement based
   * on a given object pointer
   *
   * @param obj The serializable providing the column information.
   * @return A reference to the query.
   */
  query& create(const std::string &table_name, T obj)
  {
    reset(t_query_command::CREATE);
    transition_to(state_t::QUERY_CREATE);

    sql_.append(std::make_unique<detail::create>(table_name));
    sql_.table_name(table_name);
    sql_.append(column_processor::instance().to_create_columns(obj));

    return *this;
  }

  /**
   * @brief Create a table with given name
   *
   * @param table_name The table name to be used for the statement
   * @param column_list The columns to be created
   * @return A reference to the query.
   */
  query& create(const std::string &table_name, std::initializer_list<column> column_list)
  {
    return create(table_name, std::vector<column>{column_list});
  }

  /**
   * @brief Create a table with given name
   *
   * @param table_name The table name to be used for the statement
   * @param column_list The columns to be created
   * @return A reference to the query.
   */
  query& create(const std::string &table_name, const std::vector<column>& column_list)
  {
    reset(t_query_command::CREATE);
    transition_to(state_t::QUERY_CREATE);

    sql_.append(std::make_unique<detail::create>(table_name));
    sql_.table_name(table_name);
    sql_.append(std::make_unique<matador::columns>(column_list, matador::columns::WITH_BRACKETS));

    return *this;
  }

  /**
   * Creates a drop statement.
   * 
   * @return A reference to the query.
   */
  query& drop(const std::string &table_name)
  {
    reset(t_query_command::DROP);
    transition_to(state_t::QUERY_DROP);

    sql_.append(std::make_unique<detail::drop>(table_name));
    sql_.table_name(table_name);

    return *this;
  }

  /**
   * Creates a select statement.
   * 
   * @return A reference to the query.
   */
  query& select()
  {
    return select(column_processor::instance().to_column_names(obj_, columns::WITHOUT_BRACKETS));
  }

  /**
   * Creates a select statement for the given columns.
   *
   * @param column_names A list of column names to select
   * @return A reference to the query.
   */
  query& select(const std::initializer_list<std::string> &column_names)
  {
    return select(std::make_unique<matador::columns>(column_names, columns::WITHOUT_BRACKETS));
  }

  /**
   * Appends all columns from vector
   * to a select statement.
   *
   * @param column_names The column list to select
   * @return A reference to the query.
   */
  query& select(const std::vector<std::string> &column_names)
  {
    return select(std::make_unique<matador::columns>(column_names, matador::columns::WITHOUT_BRACKETS));
  }

  /**
   * @brief Creates a select statement with the
   * given columns.
   *
   * @param cols The columns to select
   * @return A reference to the query.
   */
  template < class Type = T >
  query& select(const matador::columns &cols)
  {
    return select(std::make_unique<matador::columns>(cols));
  }

  query& select(std::unique_ptr<matador::columns> cols)
  {
    reset(t_query_command::SELECT);
    transition_to(state_t::QUERY_SELECT);

    sql_.append(std::make_unique<detail::select>());
    cols->without_brackets();
    on_select(obj_, cols);
    sql_.append(std::move(cols));

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
    return insert(column_processor::instance().to_column_names(obj, matador::columns::WITHOUT_BRACKETS));
  }

 /**
   * @brief Create an insert statement for given columns.
   * @param column_names List of column to insert
   * @return A reference to the query.
   */
  query& insert(const std::initializer_list<std::string> &column_names)
  {
    return insert(std::vector<std::string>{column_names});
  }

 /**
   * @brief Create an insert statement for given columns.
   * @param column_names List of column to insert
   * @return A reference to the query.
   */
  query& insert(const std::vector<std::string> &column_names)
  {
    return insert(std::make_unique<matador::columns>(column_names, columns::WITH_BRACKETS));
  }

 /**
   * @brief Create an insert statement for given columns.
   * @param column_names List of column to insert
   * @return A reference to the query.
   */
  query& insert(std::unique_ptr<columns> cols)
  {
    reset(t_query_command::INSERT);
    transition_to(state_t::QUERY_INSERT);

    sql_.append(std::make_unique<detail::insert>());
    cols->without_brackets();
    sql_.append(std::move(cols));

    return *this;
  }

  query& into(const std::string &table_name)
  {
    transition_to(state_t::QUERY_INTO);

    sql_.append(std::make_unique<detail::into>(table_name));
    sql_.table_name(table_name);

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
  query& values(std::initializer_list<matador::any> any_values)
  {
    transition_to(state_t::QUERY_VALUES);
    auto values = std::make_unique<detail::values>();

    // append values
    for (auto &any_value : any_values) {
      values->push_back(std::move(column_processor::instance().to_value(const_cast<any &>(any_value))));
    }
    sql_.append(std::move(values));

    return *this;
  }

  query& values(T &obj)
  {
    transition_to(state_t::QUERY_VALUES);
    // append values
    sql_.append(std::move(column_processor::instance().to_values(obj)));

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
  query& update(const std::string &table_name)
  {
    return update(table_name, obj_);
  }

  /**
   * Creates an update statement without
   * any settings. Sets for all object
   * attributes column and values.
   *
   * @param obj The object to be updated.
   * @return A reference to the query.
   */
  query& update(const std::string &table_name, T &obj)
  {
    reset(t_query_command::UPDATE);
    transition_to(state_t::QUERY_SET);

    sql_.append(std::make_unique<detail::update>());
    sql_.append(std::make_unique<detail::tablename>(table_name));
    sql_.table_name(table_name);
    sql_.append(std::make_unique<detail::set>());
    sql_.append(column_processor::instance().to_update_pairs(obj, columns::WITHOUT_BRACKETS));

    return *this;
  }

  /**
   * Creates an update statement without
   * any settings. Sets for all column value pairs
   * attributes column and values.
   *
   * @param column_values The column name and value list
   * @return A reference to the query.
   */
  query& update(const std::string &table_name,
                const std::initializer_list<std::pair<std::string, matador::any>> &column_values)
  {
    reset(t_query_command::UPDATE);
    transition_to(state_t::QUERY_SET);

    sql_.append(std::make_unique<detail::update>());
    sql_.append(std::make_unique<detail::tablename>(table_name));
    sql_.table_name(table_name);
    sql_.append(std::make_unique<detail::set>());
    sql_.append(column_processor::instance().to_update_pairs(column_values));

    return *this;
  }

  /**
   * @brief Specifies the from token of a query
   *
   * Specifies the from token of a query with
   * a table name as argument.
   *
   * @param table The name of the table
   * @return A reference to the query.
   */
  query& from(const std::string &table_name)
  {
    transition_to(state_t::QUERY_FROM);

    sql_.append(std::make_unique<detail::from>(table_name));
    sql_.table_name(table_name);

    return *this;
  }

  /**
   * @brief Specifies the from token of a query
   *
   * Specifies the from token of a query with
   * a sub query as argument.
   *
   * @param q The sub query
   * @return A reference to the query.
   */
  query& from(detail::basic_query &q)
  {
    transition_to(state_t::QUERY_FROM);

    sql_.append(std::make_unique<detail::from>(""));
    sql_.append(q.stmt());

    return *this;
  }


  /**
   * Creates a delete statement.
   * 
   * @return A reference to the query.
   */
  query& remove(const std::string &table_name)
  {
    reset(t_query_command::REMOVE);
    transition_to(state_t::QUERY_DELETE);

    sql_.append(std::make_unique<detail::remove>());
    sql_.append(std::make_unique<detail::from>(table_name));
    sql_.table_name(table_name);

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
    transition_to(state_t::QUERY_WHERE);

    sql_.append(std::make_unique<detail::where>(c));

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
    transition_to(state_t::QUERY_ORDER_BY);

    sql_.append(std::make_unique<detail::order_by>(col));

    return *this;
  }

  /**
   * Order result of select query ascending.
   *
   * @return A reference to the query.
   */
  query& asc()
  {
    transition_to(state_t::QUERY_ORDER_DIRECTION);

    sql_.append(std::make_unique<detail::asc>());

    return *this;
  }

  /**
   * Order result of select query descending.
   *
   * @return A reference to the query.
   */
  query& desc()
  {
    transition_to(state_t::QUERY_ORDER_DIRECTION);

    sql_.append(std::make_unique<detail::desc>());

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
    sql_.append(std::make_unique<detail::top>(l));
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
    transition_to(state_t::QUERY_GROUP_BY);

    sql_.append(std::make_unique<detail::group_by>(col));

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
    sql_.append(std::make_unique<detail::as>(alias));
    return *this;
  }

  /**
   * @brief Resets the query.
   *
   * @param query_command The query command to which the query is reset
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
  template<class Type = T>
  result<T> execute(connection &conn, typename std::enable_if<!std::is_same<Type, row>::value>::type* = nullptr)
  {
    return conn.execute<T>(sql_);
  }
  template<class Type = T>
  result<T> execute(connection &conn, typename std::enable_if<std::is_same<Type, row>::value>::type* = nullptr)
  {
    return conn.execute<T>(sql_, sql_.table_name(), obj_);
  }

  /**
   * Creates and returns a prepared
   * statement based on the current query.
   *
   * @param conn The connection.
   * @return The new prepared statement.
   */
  template<class Type = T>
  statement<T> prepare(connection &conn, typename std::enable_if<!std::is_same<Type, row>::value>::type* = nullptr )
  {
    return conn.prepare<T>(sql_);
  }
  template<class Type = T>
  statement<T> prepare(connection &conn, typename std::enable_if<std::is_same<Type, row>::value>::type* = nullptr )
  {
    return conn.prepare<T>(sql_, sql_.table_name(), obj_);
  }

private:
  template < class Type = T >
  void on_select(Type &/*obj*/, const std::unique_ptr<matador::columns> &/*cols*/) {}
  void on_select(row &/*obj*/ , const std::unique_ptr<matador::columns> &cols) {
    for (const auto &column : cols->columns_) {
      obj_.add_column(column.name());
    }
  }

private:
  T obj_;
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
query<row> select(columns cols);
//OOS_SQL_API query<row> select(columns cols);

}

#endif /* QUERY_HPP */
