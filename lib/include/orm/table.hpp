//
// Created by sascha on 3/21/16.
//

#ifndef OOS_TABLE_HPP
#define OOS_TABLE_HPP

#include "object/object_proxy.hpp"

#include "orm/basic_table.hpp"
#include "orm/identifier_binder.hpp"
#include "orm/identifier_column_resolver.hpp"

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
  {
    std::cout << "creating table " << name << " of type " << typeid(T).name() << '\n';
  }

  void prepare_statements(connection &conn)
  {
    query<T> q(name());
    insert_ = q.insert().prepare(conn);
    column id = detail::identifier_column_resolver::resolve<T>();
    update_ = q.update().set().where(id == 1).prepare(conn);
    delete_ = q.remove().where(id == 1).prepare(conn);
  }

  void insert_object(object_proxy *proxy)
  {
    std::cout << "inserting object (oid: " << proxy->id() << ") of type " << proxy->node()->type();
    if (proxy->has_identifier()) {
      std::cout << " and id " << *proxy->pk();
    }
    std::cout << '\n';
    insert_.bind((T*)proxy->obj());
    // Todo: check result
    insert_.execute();
  }

  void update_object(object_proxy *proxy)
  {
    T *obj = (T*)proxy->obj();
    size_t pos = update_.bind(obj);
    binder_.bind(obj, &update_, pos);
    // Todo: check result
    update_.execute();
  }

  void delete_object(object_proxy *proxy)
  {
    binder_.bind((T*)proxy->obj(), &delete_, 0);
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
  detail::identifier_binder<T> binder_;
  statement<T> insert_;
  statement<T> update_;
  statement<T> delete_;
};

}

#endif //OOS_TABLE_HPP
