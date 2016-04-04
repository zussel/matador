//
// Created by sascha on 20.03.16.
//

#ifndef OOS_PERSISTENCE_HPP
#define OOS_PERSISTENCE_HPP

#include "sql/connection.hpp"

#include "object/object_store.hpp"

#include "orm/table.hpp"

#include <memory>
#include <unordered_map>

namespace oos {

class persistence
{
public:
  typedef std::shared_ptr<basic_table> table_ptr;
  typedef std::unordered_map<std::string, table_ptr> t_table_map;

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
  void attach(const char *type, bool abstract = false, const char *parent = nullptr)
  {
    store_.attach<T>(type, abstract, parent, persistence_on_attach<T>(*this));
  }

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
  void attach(const char *type, bool abstract = false)
  {
    store_.attach<T,S>(type, abstract, persistence_on_attach<T>(*this));
  }

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

  t_table_map::iterator find_table(const std::string &type);
  t_table_map::iterator begin();
  t_table_map::iterator end();

  object_store& store();
  const object_store& store() const;

  connection& conn();
  const connection& conn() const;

private:
  template < class T >
  struct persistence_on_attach : public detail::basic_on_attach
  {
    persistence_on_attach(persistence &p) : persistence_(p) {}
    void operator()(const prototype_node *node);

    persistence &persistence_;
  };

private:
  connection connection_;
  object_store store_;

//  typedef std::unordered_map<std::type_index, table> t_table_map;
  t_table_map tables_;
};

template < class T >
void persistence::persistence_on_attach<T>::operator()(const prototype_node *node)
{
//  std::cout << "inserting table for type " << node->type() << "\n";
  persistence_.tables_.insert(std::make_pair(node->type(), std::make_shared<table<T>>(node->type())));
}

}

#endif //OOS_PERSISTENCE_HPP
