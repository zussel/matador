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
    : basic_table(name)
  { }

  virtual ~table() {}

  virtual void prepare(connection &conn)
  {
    query<T> q(name());
    insert_ = q.insert().prepare(conn);
    column id = detail::identifier_column_resolver::resolve<T>();
    update_ = q.update().set().where(id == 1).prepare(conn);
    delete_ = q.remove().where(id == 1).prepare(conn);
    select_ = q.select().prepare(conn);
  }

  virtual void create(connection &conn) {
    query<T> stmt(name());
    stmt.create().execute(conn);
  }

  virtual void drop(connection &conn) {
    query<T> stmt(name());
    stmt.drop().execute(conn);
  }

  virtual void insert(object_proxy *proxy)
  {
    insert_.bind((T*)proxy->obj());
    // Todo: check result
    insert_.execute();
  }

  virtual void update(object_proxy *proxy)
  {
    T *obj = (T*)proxy->obj();
    size_t pos = update_.bind(obj);
    binder_.bind(obj, &update_, pos);
    // Todo: check result
    update_.execute();
  }

  virtual void remove(object_proxy *proxy)
  {
    binder_.bind((T*)proxy->obj(), &delete_, 0);
    // Todo: check result
    delete_.execute();
  }

private:
  detail::identifier_binder<T> binder_;
  statement<T> insert_;
  statement<T> update_;
  statement<T> delete_;
  statement<T> select_;
};

}

#endif //OOS_TABLE_HPP
