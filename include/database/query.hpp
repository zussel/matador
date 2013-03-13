#ifndef QUERY_HPP
#define QUERY_HPP

#include "database/sql.hpp"

#include <memory>
#include <sstream>

namespace oos {

class result;
class session;
class statement;
class database;
class object;
class prototype_node;
class condition;
class object_atomizable;

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
  query& create(const std::string &name, object_atomizable *o);

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
   * @param node The serializable object used for the select statement.
   * @return A reference to the query.
   */
  query& select(object_atomizable *o);

  /**
   * Creates an insert statement based
   * on the given object.
   * 
   * @param node The object used for the insert statement.
   * @return A reference to the query.
   */
  query& insert(object *o);

  /**
   * Creates an insert statement based
   * on the given serializable object and.
   * the name of the table
   * 
   * @param node The serializable object used for the insert statement.
   * @param name The name of the table.
   * @return A reference to the query.
   */
  query& insert(object_atomizable *o, const std::string &name);

  /**
   * Creates an update statement based
   * on the given object.
   * 
   * @param node The object used for the update statement.
   * @return A reference to the query.
   */
  query& update(object *o);

  /**
   * Creates an update statement based
   * on the given serializable object and.
   * the name of the table
   * 
   * @param name The name of the table.
   * @param node The serializable object used for the update statement.
   * @return A reference to the query.
   */
  query& update(const std::string &name, object_atomizable *o);

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

  query& remove(const prototype_node &node);
  query& where(const std::string &clause);
  query& where(const condition &c);
  query& and_(const condition &c);
  query& or_(const condition &c);
  query& order_by(const std::string &by);
  query& limit(int l);
  query& group_by(const std::string &field);

  query& column(const std::string &name, sql::data_type_t type);
  query& from(const std::string &table);

  template < class T >
  query& set(const std::string &name, sql::data_type_t type, const T &val)
  {
    throw_invalid(QUERY_SET, state);

    if (state == QUERY_SET) {
      sql_.append(", ");
    }

    sql_.append(name + "=");
    std::stringstream valstr;
    valstr << val;
    sql_.append(name.c_str(), type, valstr.str());

    state = QUERY_SET;

    return *this;
  }

  result* execute();
  statement* prepare();

  query& reset();
  
private:
  void throw_invalid(state_t next, state_t current) const;

private:
  sql sql_;
  state_t state;
  database &db_;
  std::unique_ptr<statement> stmt;
};

}

#endif /* QUERY_HPP */
