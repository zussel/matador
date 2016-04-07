//
// Created by sascha on 3/21/16.
//

#ifndef OOS_TABLE_HPP
#define OOS_TABLE_HPP

#include "object/object_proxy.hpp"

#include "orm/basic_table.hpp"

#include "sql/query.hpp"

namespace oos {

class connection;

template < class T >
class table : public basic_table
{
public:
  table(const std::string &name)
    : basic_table(name, &create_table, &drop_table,
                  std::bind(&table<T>::insert_object, this, std::placeholders::_1),
                  std::bind(&table<T>::update_object, this, std::placeholders::_1),
                  std::bind(&table<T>::delete_object, this, std::placeholders::_1),
                  std::bind(&table<T>::prepare_statements, this, std::placeholders::_1)
      )
  {}

  void prepare_statements(connection &conn)
  {
    query<T> q(name());
    insert_ = q.insert().prepare(conn);
    column id("id");
    update_ = q.update().set().where(id == 1).prepare(conn);
    delete_ = q.remove().where(id == 1).prepare(conn);
  }

  void insert_object(object_proxy *proxy)
  {
    insert_.bind((T*)proxy->obj());
    // Todo: check result
    insert_.execute();
  }

  void update_object(object_proxy *proxy)
  {
    /*int pos = */update_.bind((T*)proxy->obj());
    // Todo: bind primary key
//    update_.bind();
    // Todo: check result
    update_.execute();
  }

  void delete_object(object_proxy */*proxy*/)
  {
    // Todo: bind primary key
//    delete_.bind();
    // Todo: check result
    delete_.execute();
  }

private:
  static void create_table(const std::string &name, connection &conn) {
    query<T> stmt(name);
    stmt.create().execute(conn);
  }

  static void drop_table(const std::string &name, connection &conn) {
    query<T> stmt(name);
    stmt.drop().execute(conn);
  }

private:
  statement<T> insert_;
  statement<T> update_;
  statement<T> delete_;
};

}

#endif //OOS_TABLE_HPP
