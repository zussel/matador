//
// Created by sascha on 20.03.16.
//

#ifndef OOS_PERSISTENCE_HPP
#define OOS_PERSISTENCE_HPP

#include "sql/connection.hpp"

#include "object/object_store.hpp"

#include "orm/table.hpp"

#include <unordered_map>

namespace oos {

class persistence
{
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
    store_.attach<T>(type, abstract, parent);
    tables_.insert(std::make_pair(std::type_index(typeid(T)), std::move(table(type, (T*) nullptr))));
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
    store_.attach<T,S>(type, abstract);
    tables_.insert(std::make_pair(std::type_index(typeid(T)), std::move(table(type, (T*) nullptr))));
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
    t_table_map::iterator i = tables_.find(std::type_index(typeid(T)));

    return connection_.exists(i->second.name());
  }

  void create();
  void drop();

  object_store& store();
  const object_store& store() const;

  connection& conn();
  const connection& conn() const;

private:
  connection connection_;
  object_store store_;

  typedef std::unordered_map<std::type_index, table> t_table_map;
  t_table_map tables_;
};

}

#endif //OOS_PERSISTENCE_HPP
