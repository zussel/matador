#ifndef QUERY_HPP
#define QUERY_HPP

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
  query() = default;

  ~query() = default;

  /**
   * Creates a create statement.
   * 
   * @return A reference to the query.
   */
  template<class LocalType = T, typename = typename std::enable_if<!std::is_same<LocalType, row>::value>::type>
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
  template<class LocalType = T, typename = typename std::enable_if<!std::is_same<LocalType, row>::value>::type>
  query& create(const std::string &table_name, T obj)
  {
    reset(t_query_command::CREATE);

    sql_.append(std::make_shared<detail::create>(table_name));
    sql_.table_name(table_name);

    detail::typed_column_serializer serializer;

    std::shared_ptr<columns> cols(serializer.execute(obj));

    sql_.append(cols);

    state = QUERY_CREATE;
    return *this;
  }

  /**
   * @brief Create a table with given name
   *
   * @param table_name The table name to be used for the statement
   * @param column_list The columns to be created
   * @return A reference to the query.
   */
  template<class LocalType = T, typename = typename std::enable_if<std::is_same<LocalType, row>::value>::type>
  query& create(const std::string &table_name, const std::initializer_list<std::shared_ptr<column>> &column_list)
  {
    reset(t_query_command::CREATE);

    sql_.append(std::make_shared<detail::create>(table_name));
    sql_.table_name(table_name);

    auto cols = std::make_shared<matador::columns>(matador::columns::WITH_BRACKETS);
    for (auto &&col : column_list) {
      col->build_options = t_build_options::with_type | t_build_options::with_quotes;
      cols->push_back(col);
    }

    sql_.append(cols);

    state = QUERY_CREATE;
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
    sql_.append(std::make_shared<detail::drop>(table_name));
    sql_.table_name(table_name);

    state = QUERY_DROP;
    return *this;
  }

  /**
   * Creates an insert statement
   *
   * @return A reference to the query.
   */
  template<class LocalType = T, typename = typename std::enable_if<!std::is_same<LocalType, row>::value>::type>
  query& insert(const std::string &table_name)
  {
    return insert(table_name, obj_);
  }

  /**
   * Creates an insert statement based
   * on the given object.
   *
   * @param obj The serializable used for the insert statement.
   * @return A reference to the query.
   */
  template<class LocalType = T, typename = typename std::enable_if<!std::is_same<LocalType, row>::value>::type>
  query& insert(const std::string &table_name, T &obj)
  {
    reset(t_query_command::INSERT);

    sql_.append(std::make_shared<detail::insert>(table_name));
    sql_.table_name(table_name);

    detail::column_serializer serializer(columns::WITH_BRACKETS);

    std::shared_ptr<columns> cols(serializer.execute(obj));

    sql_.append(cols);

    detail::value_serializer vserializer;

    std::shared_ptr<detail::values> vals(vserializer.execute(obj));

    sql_.append(vals);

    state = QUERY_VALUES;

    return *this;
  }

  /**
   * @brief Create an insert statement for given columns.
   * @param column_names List of column to insert
   * @return A reference to the query.
   */
  template<class LocalType = T, typename = typename std::enable_if<std::is_same<LocalType, row>::value>::type>
  query& insert(const std::string &table_name, const std::initializer_list<std::string> &column_names)
  {
    return insert(table_name, std::vector<std::string>{column_names});
  }

  /**
   * @brief Create an insert statement for given columns.
   * @param column_names List of column to insert
   * @return A reference to the query.
   */
  template<class LocalType = T, typename = typename std::enable_if<std::is_same<LocalType, row>::value>::type>
  query& insert(const std::string &table_name, const std::vector<std::string> &column_names)
  {
    reset(t_query_command::INSERT);

    sql_.append(std::make_shared<detail::insert>(table_name));
    sql_.table_name(table_name);

    auto cols = std::make_shared<matador::columns>();

    for (const auto& name : column_names) {
      cols->push_back(std::make_shared<matador::column>(name));
    }

    sql_.append(cols);

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
  template<class LocalType = T, typename = typename std::enable_if<std::is_same<LocalType, row>::value>::type>
  query& values(const std::initializer_list<matador::any> &values)
  {
    throw_invalid(QUERY_VALUES, state);

    auto vals = std::make_shared<detail::values>();

    // append values
    for (auto value : values) {
      vals->push_back(query_value_creator_.create_from_any(value));
    }
    sql_.append(vals);

    state = QUERY_VALUES;

    return *this;
  }

  /**
   * Creates a select statement.
   * 
   * @return A reference to the query.
   */
  template<class LocalType = T, typename = typename std::enable_if<!std::is_same<LocalType, row>::value>::type>
  query& select()
  {
    reset(t_query_command::SELECT);

    throw_invalid(QUERY_SELECT, state);
    sql_.append(std::make_shared<detail::select>());

    detail::column_serializer serializer(columns::WITHOUT_BRACKETS);

    std::shared_ptr<columns> cols(serializer.execute(obj_));

    sql_.append(cols);

    state = QUERY_SELECT;
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
    return select(std::vector<std::string>{column_names});
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
    reset(t_query_command::SELECT);

    throw_invalid(QUERY_SELECT, state);
    sql_.append(std::make_shared<detail::select>());

    auto cols = std::make_shared<matador::columns>(column_names, matador::columns::WITHOUT_BRACKETS);

    prepare_select_prototype(obj_, cols);

    sql_.append(cols);

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
    sql_.append(std::make_shared<detail::select>());

    cols.without_brackets();
    auto cols_ptr = std::make_shared<matador::columns>(cols);
    sql_.append(cols_ptr);

    prepare_select_prototype(obj_, cols_ptr);

    state = QUERY_SELECT;
    return *this;
  }

  template<class Type>
  void prepare_select_prototype(Type & /*obj*/, const std::shared_ptr<columns> &/*cols*/) {}
  void prepare_select_prototype(row &obj, const std::shared_ptr<columns> &cols) {
    for (const auto &column : cols->columns_) {
      obj.add_column(column->name);
    }
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
  template<class LocalType = T, typename = typename std::enable_if<!std::is_same<LocalType, row>::value>::type>
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
  template<class LocalType = T, typename = typename std::enable_if<!std::is_same<LocalType, row>::value>::type>
  query& update(const std::string &table_name, T &obj)
  {
    reset(t_query_command::UPDATE);

    sql_.append(std::make_shared<detail::update>());
    sql_.append(std::make_shared<detail::tablename>(table_name));
    sql_.table_name(table_name);
    sql_.append(std::make_shared<detail::set>());
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
   * @param column_values The column name and value list
   * @return A reference to the query.
   */
  query& update(const std::string &table_name, const std::initializer_list<std::pair<std::string, matador::any>> &column_values)
  {
    reset(t_query_command::UPDATE);

    sql_.append(std::make_shared<detail::update>());
    sql_.append(std::make_shared<detail::tablename>(table_name));
    sql_.table_name(table_name);
    sql_.append(std::make_shared<detail::set>());

    for (auto column_value : column_values) {
      query_value_column_processor_.execute(column_value);
    }

    sql_.append(update_columns_);

    state = QUERY_SET;
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
    throw_invalid(QUERY_FROM, state);

    sql_.append(std::make_shared<detail::from>(table_name));
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
    throw_invalid(QUERY_FROM, state);

    sql_.append(std::make_shared<detail::from>(""));
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

    sql_.append(std::make_shared<detail::remove>());
    sql_.append(std::make_shared<detail::from>(table_name));
    sql_.table_name(table_name);

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

    sql_.append(std::make_shared<detail::where>(c));

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

    sql_.append(std::make_shared<detail::order_by>(col));

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

    sql_.append(std::make_shared<detail::asc>());

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

    sql_.append(std::make_shared<detail::desc>());

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
    sql_.append(std::make_shared<detail::top>(l));
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
    sql_.append(std::make_shared<detail::as>(alias));
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

    sql_.append(std::make_shared<detail::group_by>(col));

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
    sql_.append(make_column(column, type, index));
    state = QUERY_COLUMN;
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
  result<T> execute(connection &conn)
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
  statement<T> prepare(connection &conn)
  {
    return conn.prepare<T>(sql_, sql_.table_name(), obj_);
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

}

#endif /* QUERY_HPP */
