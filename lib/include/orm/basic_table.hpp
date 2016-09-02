//
// Created by sascha on 3/24/16.
//

#ifndef OOS_BASIC_TABLE_HPP
#define OOS_BASIC_TABLE_HPP

#include "object/identifier_proxy_map.hpp"

#include <string>
#include <functional>

namespace oos {

namespace detail {

template < class T >
class relation_resolver;

}

class connection;
class object_proxy;
class object_store;
class persistence;

class basic_table
{
public:
  typedef std::shared_ptr<basic_table> table_ptr;
  typedef std::unordered_map<std::string, table_ptr> t_table_map;

public:
  basic_table(prototype_node *node, persistence &p);

  virtual ~basic_table();

  std::string name() const;
  virtual void create(connection &conn) = 0;
  virtual void drop(connection &conn) = 0;
  virtual void load(object_store &p) = 0;
  virtual void prepare(connection &conn) = 0;
  virtual void insert(object_proxy *proxy) = 0;
  virtual void update(object_proxy *proxy) = 0;
  virtual void remove(object_proxy *proxy) = 0;

  bool is_loaded() const;

protected:
  template < class T >
  friend class detail::relation_resolver;
  template < class T >
  friend class relation_table;

protected:
  t_table_map::iterator find_table(const std::string &type);
  t_table_map::iterator begin_table();
  t_table_map::iterator end_table();

  void append_relation_items(const std::string &id);

  persistence &persistence_;

  detail::t_identifier_map identifier_proxy_map_;
  // store own proxy for relation resolution of relation tables
  // (when this table is loaded before relation table)
  detail::t_identifier_map identifier_self_proxy_map_;

  unsigned has_many_count_ = 0;

  std::shared_ptr<basic_identifier> relation_owner_id_;

  typedef std::unordered_map<std::string, detail::t_identifier_multimap> t_relation_item_map;
  t_relation_item_map has_many_relations_;

  bool is_loaded_ = false;

private:
  prototype_node *node_;
};

}

#endif //OOS_BASIC_TABLE_HPP
