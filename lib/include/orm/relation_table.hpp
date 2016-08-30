/**
 * @author sascha on 4/18/16.
 */

#include "orm/basic_table.hpp"

#include "sql/query.hpp"

#include "object/basic_has_many.hpp"

#include "tools/basic_identifier.hpp"

#include <memory>

#ifndef OOS_RELATION_TABLE_HPP
#define OOS_RELATION_TABLE_HPP

namespace oos {

template < class T >
class relation_table : public basic_table
{
public:

  typedef has_many_item<T> relation_type;

  relation_table(prototype_node *node, persistence &p, const relation_type &relation,
                 const std::string &owner_id_field, const std::string &item_id_field)
    : basic_table(node, p)
    , item_(relation)
    , owner_id_column_(owner_id_field)
    , item_id_column_(item_id_field)
    , resolver_(*this)
  {
    item_.owner_id(owner_id_column_);
    item_.item_id(item_id_column_);
  }

  virtual void create(connection &conn) override
  {
    query<relation_type> stmt(name());
    stmt.create(&item_).execute(conn);
  }

  virtual void drop(connection &conn) override
  {
    query<relation_type> stmt(name());
    stmt.drop().execute(conn);
  }

  virtual void prepare(connection &conn) override
  {
    query<relation_type> q(name());

    select_all_ = q.select({owner_id_column_, item_id_column_}).prepare(conn);
    insert_ = q.insert(&item_).prepare(conn);

    column owner_id(owner_id_column_);
    column item_id(item_id_column_);

    update_ = q.update().set(&item_).where(owner_id == 1 && item_id == 1).limit(1).prepare(conn);
    delete_ = q.remove().where(owner_id == 1 && item_id == 1).limit(1).prepare(conn);
  }

  virtual void load(object_store &store) override
  {
    if (is_loaded_) {
      return;
    }
    auto res = select_all_.execute();

    // set explicit creator function
    relation_type item(item_);
    auto func = [item]() {
      relation_type *i = new relation_type(item.owner_id(), item.item_id(), item.owner()->clone());
      return i;
    };
    res.creator(func);

    auto first = res.begin();
    auto last = res.end();

    while (first != last) {
      // create new proxy of relation object
      proxy_.reset(new object_proxy(first.release()));
      ++first;
      object_proxy *proxy = store.insert<T>(proxy_.release(), false);
      resolver_.resolve(proxy, &store);
    }

    is_loaded_ = true;
  }

  virtual void insert(object_proxy *proxy) override
  {
    insert_.bind((relation_type*)proxy->obj(), 0);
    // Todo: check result
    insert_.execute();
  }

  virtual void update(object_proxy *proxy) override
  {
    size_t pos = update_.bind((relation_type*)proxy->obj(), 0);
    update_.bind((relation_type*)proxy->obj(), pos);

    update_.execute();
  }

  virtual void remove(object_proxy *proxy) override
  {
    delete_.bind((relation_type*)proxy->obj(), 0);
    delete_.execute();
  }

private:
  relation_type item_;

  statement<relation_type> select_all_;
  statement<relation_type> insert_;
  statement<relation_type> update_;
  statement<relation_type> delete_;

  std::string owner_id_column_;
  std::string item_id_column_;

  detail::relation_resolver<T> resolver_;

  std::unique_ptr<object_proxy> proxy_;
};

}
#endif //OOS_RELATION_TABLE_HPP
