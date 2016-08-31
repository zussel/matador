//
// Created by sascha on 20.03.16.
//

#ifndef OOS_PERSISTENCE_HPP
#define OOS_PERSISTENCE_HPP

#include "sql/connection.hpp"

#include "object/object_store.hpp"

#include "orm/table.hpp"
#include "orm/relation_table.hpp"

#include <memory>
#include <unordered_map>

namespace oos {

namespace detail {

template < class T >
struct persistence_on_attach;

}

class persistence
{
public:
  typedef basic_table::table_ptr table_ptr;
  typedef basic_table::t_table_map t_table_map;

public:
  explicit persistence(const std::string &dns);
  ~persistence();

  /**
   * Inserts a new object prototype into the prototype tree. The prototype
   * constist of a unique type name. To know where the new
   * prototype is inserted into the hierarchy the type name of the parent
   * node is also given.
   *
   * @tparam T       The type of the prototype node
   * @param type     The unique name of the type.
   * @param abstract Indicates if the producers serializable is treated as an abstract node.
   * @param parent   The name of the parent type.
   * @return         Returns new inserted prototype iterator.
   */
  template<class T>
  void attach(const char *type, bool abstract = false, const char *parent = nullptr);

  /**
   * Inserts a new object prototype into the prototype tree. The prototype
   * constist of a unique type name. To know where the new
   * prototype is inserted into the hierarchy the type name of the parent
   * node is also given.
   * parameter.
   *
   * @tparam T       The type of the prototype node
   * @tparam S       The type of the parent prototype node
   * @param type     The unique name of the type.
   * @param abstract Indicates if the producers serializable is treated as an abstract node.
   * @return         Returns new inserted prototype iterator.
   */
  template<class T, class S>
  void attach(const char *type, bool abstract = false);

  /**
   * Checks if the given entity as
   * table exists
   *
   * @tparam T entity type class
   * @return True if table exists
   */
  template < class T >
  bool exists()
  {
    t_table_map::iterator i = tables_.find(store_.type<T>());

    if (i == tables_.end()) {
      return false;
    }
    return connection_.exists(i->second->name());
  }

  void create();
  void drop();

  /**
   * clears all objects from object store
   */
  void clear();

  t_table_map::iterator find_table(const std::string &type);
  t_table_map::iterator begin();
  t_table_map::iterator end();

  object_store& store();
  const object_store& store() const;

  connection& conn();
  const connection& conn() const;

private:
  template < class T >
  friend struct detail::persistence_on_attach;

private:
  connection connection_;
  object_store store_;

//  typedef std::unordered_map<std::type_index, table> t_table_map;
  t_table_map tables_;
};

namespace detail {

struct basic_persistence_on_attach : public detail::basic_on_attach
{
  basic_persistence_on_attach(persistence &p) : persistence_(p) {}
  basic_persistence_on_attach(const basic_persistence_on_attach &x) : persistence_(x.persistence_) {}
  basic_persistence_on_attach& operator=(const basic_persistence_on_attach &x) { persistence_ = x.persistence_; return *this; }

  std::reference_wrapper<persistence> persistence_;
};

template < class T >
struct persistence_on_attach : public basic_persistence_on_attach
{
  using basic_persistence_on_attach::basic_persistence_on_attach;

  persistence_on_attach(persistence &p) : basic_persistence_on_attach(p) {}

  template < class V >
  persistence_on_attach(const persistence_on_attach<V> &x);

  template < class V >
  persistence_on_attach& operator=(const persistence_on_attach<V> &x) { persistence_ = x.persistence_; return *this; }

  void operator()(prototype_node *node) const;
};

//template <>
template < class T >
struct persistence_on_attach<has_many_item<T>> : public basic_persistence_on_attach
{
  using basic_persistence_on_attach::basic_persistence_on_attach;

  typedef has_many_item<T> relation_type;

  persistence_on_attach(persistence &p) : basic_persistence_on_attach(p) {}

  persistence_on_attach(const persistence_on_attach &x)
    : basic_persistence_on_attach(x)
    , relation_(x.relation_)
  { }

  template < class V >
  persistence_on_attach(const persistence_on_attach<V> &x);

  template < class V >
  persistence_on_attach& operator=(const persistence_on_attach<V> &x) { persistence_ = x.persistence_; return *this; }

  void operator()(prototype_node *node) const;


  template < class V >
  void serialize(T &obj)
  {
    oos::access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const char *, identifier<V> &)
  {
    std::shared_ptr<basic_identifier> id(new identifier<V>);
    id->as_value(true);
    relation_.owner(id);
  }

  template < class V >
  void serialize(const char*, V &) {}

  template < class V >
  void serialize(const char*, V &, size_t) {}

  template < class HAS_ONE >
  void serialize(const char*, HAS_ONE &, cascade_type) {}

  template < class HAS_MANY >
  void serialize(const char*, HAS_MANY &, const char *owner_field, const char *item_field)
  {
    owner_id_column_.assign(owner_field);
    item_id_column_.assign(item_field);
  }

  relation_type relation_;
  std::string owner_type_;
  std::string owner_id_column_;
  std::string item_id_column_;
};

template<class T>
template<class V>
persistence_on_attach<T>::persistence_on_attach(const persistence_on_attach<V> &x)
  : basic_persistence_on_attach(x.persistence_)
{ }

template<class T>
void persistence_on_attach<T>::operator()(prototype_node *node) const
{
  persistence_.get().tables_.insert(std::make_pair(node->type(), std::make_shared<table<T>>(node, persistence_)));
}

template<class T>
template<class V>
persistence_on_attach<has_many_item<T>>::persistence_on_attach(const persistence_on_attach<V> &x)
  : basic_persistence_on_attach(x.persistence_)
{
  V owner;
  owner_type_ = persistence_.get().store().find(typeid(V).name())->type();
  oos::access::serialize(*this, owner);
}

//template <>
template <class T>
void persistence_on_attach<has_many_item<T>>::operator()(prototype_node *node) const
{
  persistence_.get().tables_.insert(std::make_pair(
    node->type(), std::make_shared<relation_table<typename relation_type::object_type>>(
      node, persistence_, relation_, owner_type_, owner_id_column_, item_id_column_
    )));
}

}

template<class T>
void persistence::attach(const char *type, bool abstract, const char *parent)
{
  store_.attach<T, detail::persistence_on_attach>(type, abstract, parent, detail::persistence_on_attach<T>(*this));
}

template<class T, class S>
void persistence::attach(const char *type, bool abstract)
{
  store_.attach<T,S, detail::persistence_on_attach>(type, abstract, detail::persistence_on_attach<T>(*this));
}

}

#endif //OOS_PERSISTENCE_HPP
