#ifndef QUERY_HPP
#define QUERY_HPP

#include "sql.hpp"

#include <memory>
#include <sstream>

class result;
class session;
class statement;
class object;

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
    QUERY_ORDERBY,
    QUERY_GROUPBY,
    QUERY_EXECUTED,
    QUERY_PREPARED,
    QUERY_BOUND
  };

public:
  query(session &s);
  ~query();

  query& create(object *o);
  query& drop(object *o);
  query& select(object *o);
  query& insert(object *o);
  query& update(object *o);
  query& remove(object *o);
  query& where(const std::string &clause);
  query& order_by(const std::string &by);
  query& limit(int l);
  query& group_by(const std::string &field);

  query& select();
  query& column(const std::string &name, sql::data_type_t type);
  query& from(const std::string &table);

  query& update(const std::string &table);
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
  session &session_;
  std::unique_ptr<statement> stmt;
};

#endif /* QUERY_HPP */
