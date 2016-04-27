//
// Created by sascha on 3/21/16.
//

#ifndef OOS_TABLE_HPP
#define OOS_TABLE_HPP

#include "object/object_proxy.hpp"
#include "object/object_store.hpp"

#include "orm/basic_table.hpp"
#include "orm/identifier_binder.hpp"
#include "orm/identifier_column_resolver.hpp"
#include "orm/relation_resolver.hpp"

#include "sql/query.hpp"

namespace oos {

class connection;

template < class T >
class table : public basic_table
{
public:
  table(const std::string &name, persistence &p)
    : basic_table(name, p)
    , resolver_(*this)
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

  virtual void create(connection &conn) override
  {
    query<T> stmt(name());
    stmt.create().execute(conn);
  }

  virtual void drop(connection &conn) override
  {
    query<T> stmt(name());
    stmt.drop().execute(conn);
  }

  virtual void load(object_store &store) override
  {
    auto result = select_.execute();

    auto first = result.begin();
    auto last = result.end();

    while (first != last) {
      std::unique_ptr<T> item(first.release());
      ++first;
      resolver_.resolve(item.get(), &store);
      store.insert<T>(item.release());
    }

  }

  virtual void insert(object_proxy *proxy) override
  {
    insert_.bind((T*)proxy->obj(), 0);
    // Todo: check result
    insert_.execute();
  }

  virtual void update(object_proxy *proxy) override
  {
    T *obj = (T*)proxy->obj();
    size_t pos = update_.bind(obj, 0);
    binder_.bind(obj, &update_, pos);
    // Todo: check result
    update_.execute();
  }

  virtual void remove(object_proxy *proxy) override
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

  detail::relation_resolver<T> resolver_;
};

}

#endif //OOS_TABLE_HPP
