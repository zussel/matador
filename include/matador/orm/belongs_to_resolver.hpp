#ifndef MATADOR_BELONGS_TO_RESOLVER_HPP
#define MATADOR_BELONGS_TO_RESOLVER_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"

#include "matador/object/prototype_node.hpp"

#include "matador/orm/basic_table.hpp"

#include <cstddef>

namespace matador {
namespace detail {

/// @cond MATADOR_DEV

template < class T >
class belongs_to_resolver
{
public:
  typedef std::map<std::string, std::shared_ptr<matador::basic_table>> t_table_map;

public:
  explicit belongs_to_resolver(basic_table &tbl) : table_(tbl) {}

  t_table_map resolve()
  {
    table_map_.clear();

    auto node = table_.persistence_unit().store().template find<T>();

    if (node == table_.persistence_unit().store().end()) {
      // Todo: introduce throw_orm_exception
      throw std::logic_error("belongs_to_resolver::resolve: no owner table " + std::string(typeid(T).name()) + " found");
    }
    prototype_node* n = node.get();
    T* proto = n->prototype<T>();
    serialize(*proto);

    return table_map_;
  }

  template<class V>
  void serialize(V &obj)
  {
    matador::access::process(*this, obj);
  }

  template < class V >
  void on_primary_key(const char *, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &, size_t /*size*/) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template<class V>
  void on_attribute(const char *, V &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, char *, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) { }

  template<class V>
  void on_belongs_to(const char *id, object_ptr<V> &, cascade_type)
  {
    auto it = table_.find_table<V>();
    if (it != table_.end_table()) {
      table_map_.insert(std::make_pair(id, it->second));
    }
  }

  template<class V>
  void on_has_one(const char *id, object_ptr<V> &, cascade_type)
  {
    auto it = table_.find_table<V>();
    if (it != table_.end_table()) {
      table_map_.insert(std::make_pair(id, it->second));
    }
  }

  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  basic_table &table_;

  t_table_map table_map_;
};

/// @endcond

}
}
#endif //MATADOR_BELONGS_TO_RESOLVER_HPP
