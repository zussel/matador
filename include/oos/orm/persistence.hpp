//
// Created by sascha on 20.03.16.
//

#ifndef OOS_PERSISTENCE_HPP
#define OOS_PERSISTENCE_HPP

#ifdef _MSC_VER
#ifdef oos_orm_EXPORTS
#define OOS_ORM_API __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define OOS_ORM_API __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_ORM_API
#endif

#include "oos/sql/connection.hpp"

#include "oos/object/object_store.hpp"

#include "oos/orm/table.hpp"
#include "oos/orm/relation_table.hpp"

#include <memory>
#include <unordered_map>

namespace oos {

namespace detail {

/// @cond OOS_DEV

template < class T >
struct persistence_on_attach;

/// @endcond

}

/**
 * @brief Represents the persistence layer for a database and an object_store
 *
 * This class acts as mediator between the database connection
 * and the object_store.
 * It attaches the object prototypes to the object_store and
 * drops or creates the corresponding tables on the database.
 *
 * It also holds a map of all created tables, checks if a table exists
 * and is able to clear the object store.
 */
class OOS_ORM_API persistence
{
public:
  typedef basic_table::table_ptr table_ptr;      /**< Shortcut to the table shared pointer */
  typedef basic_table::t_table_map t_table_map;  /**< Shortcut to map of table shared pointer */

public:
  /**
   * @brief Creates a new persistence object
   *
   * Creates a new persistence object for the given
   * database connection string.
   *
   * @param dns The database connection string
   */
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

  /**
   * @brief Creates all tables.
   *
   * Creates all tables currently attached
   * to the persistence object.
   */
  void create();

  /**
   * @brief Drops all tables.
   *
   * Drops all tables currently attached
   * to the persistence object.
   */
  void drop();

  /**
   * clears all objects from object store
   */
  void clear();

  /**
   * @brief Finds a table by its name
   *
   * Finds a table by its name. If the name is unknown
   * map::end is returned.
   *
   * @param type The name of the table
   * @return The iterator of the table or end
   */
  t_table_map::iterator find_table(const std::string &type);

  /**
   * @brief Returns the begin of the table map
   *
   * @return The begin of the table map
   */
  t_table_map::iterator begin();

  /**
   * @brief Returns the end of the table map
   *
   * @return The end of the table map
   */
  t_table_map::iterator end();

  /**
   * @brief Return a reference to the underlaying object_store
   *
   * @return A reference to the object_store.
   */
  object_store& store();

  /**
   * @brief Return a const reference to the underlaying object_store
   *
   * @return A const reference to the object_store.
   */
  const object_store& store() const;

  /**
   * @brief Return a reference to the underlaying database connection
   *
   * @return A reference to the database connection.
   */
  connection& conn();

  /**
   * @brief Return a const reference to the underlaying database connection
   *
   * @return A const reference to the database connection.
   */
  const connection& conn() const;

private:
  template < class T >
  friend struct detail::persistence_on_attach;

private:
  connection connection_;
  object_store store_;

  t_table_map tables_;
};

namespace detail {

/// @cond OOS_DEV

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
  void serialize(const char *id, HAS_MANY &, const char *owner_field, const char *item_field)
  {
    owner_id_column_.assign(owner_field);
    item_id_column_.assign(item_field);
    relation_id_.assign(id);
  }

  relation_type relation_;
  std::string owner_type_;
  std::string relation_id_;
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
  if (persistence_.get().tables_.find(node->type()) != persistence_.get().tables_.end()) {
    return;
  }
  persistence_.get().tables_.insert(std::make_pair(
    node->type(), std::make_shared<relation_table<typename relation_type::object_type>>(
      node, persistence_, relation_, owner_type_, relation_id_, owner_id_column_, item_id_column_
    )));
}

/// @endcond

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
