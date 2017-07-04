/**
 * @author sascha on 4/18/16.
 */

#include "matador/orm/basic_table.hpp"

#include "matador/sql/query.hpp"

#include "matador/object/basic_has_many.hpp"
#include "matador/object/object_proxy_accessor.hpp"
#include "matador/object/prototype_node.hpp"

#include "matador/utils/basic_identifier.hpp"

#include <memory>

#ifndef OOS_RELATION_TABLE_HPP
#define OOS_RELATION_TABLE_HPP

namespace matador {

/// @cond MATADOR_DEV

template < class T >
class table : public basic_table, public detail::object_proxy_accessor
{
public:
  relation_table(prototype_node &node, persistence &p)
    : basic_table(node, p)
    , resolver_(*this)
  {

    if (!node.is_relation_node()) {
      throw_object_exception("node " << node.type() << " isn't a relation node");
    }
  }

  virtual void create(connection &conn) override
  {
    query<T> stmt(name());
    stmt.create(*this->node_.prototype<T>()).execute(conn);
  }

  virtual void drop(connection &conn) override
  {
    query<T> stmt(name());
    stmt.drop().execute(conn);
  }

  virtual void prepare(connection &conn) override
  {
    query<T> q(name());

    select_all_ = q.select().prepare(conn);
    T *proto = this->node_.prototype<T>();
//    select_all_ = q.select({owner_id_column_, item_id_column_}).prepare(conn);
    insert_ = q.insert(*proto).prepare(conn);

    column owner_id(proto->left_column());
    column item_id(proto->right_column());

    update_ = q.update(*proto).where(owner_id == 1 && item_id == 1).limit(1).prepare(conn);
    delete_ = q.remove().where(owner_id == 1 && item_id == 1).limit(1).prepare(conn);

    // find owner table
//    auto tid = find_table(owner_type_);
//    if (tid == end_table()) {
//      // Todo: introduce throw_orm_exception
//      throw std::logic_error("no owner table " + owner_type_ + " found");
//    }
//    owner_table_ = tid->second;
  }

  virtual void load(object_store &store) override
  {
    if (is_loaded_) {
      return;
    }
    auto res = select_all_.execute();

    // set explicit creator function
//    T item(item_);
    prototype_node &node = this->node_;

    auto func = [&node]() {

      T *i = node.create<T>();
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

      // append item to owner table
//      auto i = owner_table_->has_many_relations_.find(relation_id_);
//      if (i == owner_table_->has_many_relations_.end()) {
//        i = owner_table_->has_many_relations_.insert(
//        std::make_pair(relation_id_, detail::t_identifier_multimap())).first;
//      }
//      i->second.insert(std::make_pair(proxy->obj<relation_type>()->owner(), proxy));
    }

//    if (owner_table_->is_loaded()) {
//      // append items
//      owner_table_->append_relation_items(relation_id_, identifier_proxy_map_, owner_table_->has_many_relations_);
//    }

    is_loaded_ = true;
  }

  virtual void insert(object_proxy *proxy) override
  {
    insert_.bind(0, static_cast<T*>(proxy->obj()));
    // Todo: check result
    insert_.execute();
  }

  virtual void update(object_proxy *proxy) override
  {
    size_t pos = update_.bind(0, static_cast<T*>(proxy->obj()));
    update_.bind(pos, static_cast<T*>(proxy->obj()));

    update_.execute();
  }

  virtual void remove(object_proxy *proxy) override
  {
    delete_.bind(0, static_cast<T*>(proxy->obj()));
    delete_.execute();
  }

private:
  statement<T> select_all_;
  statement<T> insert_;
  statement<T> update_;
  statement<T> delete_;

  detail::relation_resolver<T> resolver_;

  std::unique_ptr<object_proxy> proxy_;

  table_ptr owner_table_;
};

/// @endcond

}
#endif //OOS_RELATION_TABLE_HPP
