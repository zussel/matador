/**
 * @author sascha on 4/18/16.
 */

#include "matador/orm/basic_table.hpp"

#include "matador/sql/query.hpp"

#include "matador/object/basic_has_many.hpp"
#include "matador/object/object_proxy_accessor.hpp"

#include "matador/utils/basic_identifier.hpp"

#include <memory>

#ifndef OOS_RELATION_TABLE_HPP
#define OOS_RELATION_TABLE_HPP

namespace matador {

/// @cond OOS_DEV

template < class T >
class relation_table : public basic_table, public detail::object_proxy_accessor
{
public:

  typedef has_many_item<T> relation_type;

  relation_table(prototype_node *node, persistence &p, const relation_type &relation,
                 const std::string &owner_type, const std::string &relation_id,
                 const std::string &owner_id_field, const std::string &item_id_field)
    : basic_table(node, p)
    , item_(relation)
    , owner_id_column_(owner_id_field)
    , item_id_column_(item_id_field)
    , owner_type_(owner_type)
    , relation_id_(relation_id)
    , resolver_(*this)
  {
    item_.owner_id(owner_id_column_);
    item_.item_id(item_id_column_);
  }

  virtual void create(connection &conn) override
  {
    query<relation_type> stmt(name());
    stmt.create(item_).execute(conn);
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
    insert_ = q.insert(item_).prepare(conn);

    column owner_id(owner_id_column_);
    column item_id(item_id_column_);

    update_ = q.update(item_).where(owner_id == 1 && item_id == 1).limit(1).prepare(conn);
    delete_ = q.remove().where(owner_id == 1 && item_id == 1).limit(1).prepare(conn);

    // find owner table
    auto tid = find_table(owner_type_);
    if (tid == end_table()) {
      // Todo: introduce throw_orm_exception
      throw std::logic_error("no owner table " + owner_type_ + " found");
    }
    owner_table_ = tid->second;
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
      object_proxy *proxy = store.insert<relation_type>(proxy_.release(), false);
      resolver_.resolve(proxy, &store);

      // append item to owner table
      auto i = owner_table_->has_many_relations_.find(relation_id_);
      if (i == owner_table_->has_many_relations_.end()) {
        i = owner_table_->has_many_relations_.insert(
        std::make_pair(relation_id_, detail::t_identifier_multimap())).first;
      }
      i->second.insert(std::make_pair(proxy->obj<relation_type>()->owner(), proxy));
    }

    if (owner_table_->is_loaded()) {
      // append items
      owner_table_->append_relation_items(relation_id_, identifier_proxy_map_, owner_table_->has_many_relations_);
    }

    is_loaded_ = true;
  }

  virtual void insert(object_proxy *proxy) override
  {
    insert_.bind(0, (relation_type*)proxy->obj());
    // Todo: check result
    insert_.execute();
  }

  virtual void update(object_proxy *proxy) override
  {
    size_t pos = update_.bind(0, static_cast<relation_type*>(proxy->obj()));
    update_.bind(pos, static_cast<relation_type*>(proxy->obj()));

    update_.execute();
  }

  virtual void remove(object_proxy *proxy) override
  {
    delete_.bind(0, static_cast<relation_type*>(proxy->obj()));
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
  std::string owner_type_;
  std::string relation_id_;

  detail::relation_resolver<relation_type> resolver_;

  std::unique_ptr<object_proxy> proxy_;

  table_ptr owner_table_;
};

/// @endcond

}
#endif //OOS_RELATION_TABLE_HPP
