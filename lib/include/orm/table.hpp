//
// Created by sascha on 3/21/16.
//

#ifndef OOS_TABLE_HPP
#define OOS_TABLE_HPP

#include "object/object_proxy.hpp"

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
    , insert_(&insert_object<T>)
  {
  }

  std::string name() const;
  void create(connection &conn);
  void drop(connection &conn);
  void insert(object_proxy *proxy, connection &conn)
  {
    insert_(proxy, name_, conn);
  }

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

  template < class T >
  static void insert_object(object_proxy *proxy, const std::string &name, connection &conn) {
    T *obj = (T*)proxy->obj();
    static query<T> q(name);
    static statement<T> stmt(q.insert(obj).prepare(conn));
    stmt.bind(obj);
    stmt.execute();
  }

  template < class T >
  static void update_object(object_proxy *proxy, const std::string &name, connection &conn) {
    T *obj = (T*)proxy->obj();
    static query<T> q(name);
    static statement<T> stmt(q.update(obj).where().prepare(conn));
    stmt.bind(obj);
    stmt.execute();
  }

  typedef void(*t_create_func)(const std::string&, connection &);
  typedef void(*t_drop_func)(const std::string&, connection &);
  typedef void(*t_insert_object)(object_proxy *, const std::string&, connection &);
  typedef void(*t_update_object)(object_proxy *, const std::string&, connection &);

private:
  std::string name_;

  t_create_func create_;
  t_drop_func drop_;
  t_insert_object insert_;
  t_update_object update_;
};

}

#endif //OOS_TABLE_HPP
