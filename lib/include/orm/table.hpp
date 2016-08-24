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
  table(prototype_node *node, persistence &p)
    : basic_table(node, p)
    , resolver_(*this)
  { }

  virtual ~table() {}

  virtual void prepare(connection &conn) override
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
      // try to find object proxy by id
      std::shared_ptr<basic_identifier> id(identifier_resolver_.resolve_object(first.get()));

      detail::t_identifier_map::iterator i = identifier_proxy_map_.find(id);
      if (i != identifier_proxy_map_.end()) {
        // use proxy;
        proxy_.reset(i->second);
        proxy_->reset(first.release(), false);
        identifier_proxy_map_.erase(i);
      } else {
        // create new proxy
        proxy_.reset(new object_proxy(first.release()));
      }

      ++first;
      object_proxy *proxy = store.insert<T>(proxy_.release(), false);
      resolver_.resolve(proxy->obj<T>(), &store);
    }
    // load all relation tables belonging to this table
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

  std::unique_ptr<object_proxy> proxy_;

  identifier_resolver<T> identifier_resolver_;
};

}

#endif //OOS_TABLE_HPP
