//
// Created by sascha on 3/21/16.
//

#ifndef OOS_TABLE_HPP
#define OOS_TABLE_HPP

#include "sql/query.hpp"

namespace oos {

class connection;

class table
{
public:
  template < class T >
  table(const std::string &name, T*)
    : name_(name)
    , create_(&create_table<T>)
    , drop_(&drop_table<T>)
  {}

  std::string name() const;
  void create(connection &conn);
  void drop(connection &conn);

private:
  template < class T >
  static void create_table(const std::string &name, connection &conn) {
    query<T> stmt(name);
    stmt.create().execute(conn);
  }

  template < class T >
  static void drop_table(const std::string &name, connection &conn) {
    query<T> stmt(name);
    stmt.drop().execute(conn);
  }

  typedef void(*t_create_func)(const std::string&, connection &);
  typedef void(*t_drop_func)(const std::string&, connection &);

private:
  std::string name_;

  t_create_func create_;
  t_drop_func drop_;
};

}

#endif //OOS_TABLE_HPP
