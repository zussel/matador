#ifndef MATADOR_OBJECT_TYPE_REGISTRY_HPP
#define MATADOR_OBJECT_TYPE_REGISTRY_HPP

#include "matador/utils/identifier_setter.hpp"

#include "matador/object/object_type_registry_entry_base.hpp"
#include "matador/object/object_proxy.hpp"
#include "matador/object/prototype_node.hpp"
#include "matador/object/object_serializer.hpp"
#include "matador/object/object_deserializer.hpp"

#include <typeindex>
#include <unordered_map>

namespace matador {

class object_store;

namespace detail {

template < typename T >
class object_type_registry_entry : public object_type_registry_entry_base
{
public:
  object_type_registry_entry(object_store *store, prototype_node *node)
    : object_type_registry_entry_base(std::type_index(typeid(T)), store, node) {}

  void sync_id(object_proxy *proxy) const override
  {
    identifier_setter<unsigned long long>::assign(proxy->id(), proxy->obj<T>());
  }

  void restore(object_proxy *proxy, byte_buffer &buffer, object_deserializer &deserializer) const override
  {
    deserializer.restore(proxy->obj<T>(), &buffer, store());
  }
  void backup(object_proxy *proxy, byte_buffer &buffer, object_serializer &serializer) const override
  {
    serializer.backup(proxy->obj<T>(), &buffer);
  }

  void create_object(object_proxy *) const override
  {

  }

  void insert_object(object_proxy *proxy) const override
  {
    matador::access::serialize(inserter(), *proxy->obj<T>());
  }

  void delete_object(object_proxy *proxy) const override
  {
    matador::access::serialize(deleter(), *proxy->obj<T>());
  }

  identifier resolve_identifier(object_proxy *proxy) const override
  {
    if (proxy->obj()) {
      return null_identifier;
    }
    return resolver_.resolve(proxy->obj<T>());
  }

private:
  identifier_resolver<T> resolver_;
};

class object_type_registry
{
public:
  template<class Type>
  void register_type(object_store *store, prototype_node *node) {
    type_registry_.insert(std::make_pair(std::type_index(typeid(Type)), std::make_shared<object_type_registry_entry<Type>>(store, node)));
  }

  template<class Type>
  std::shared_ptr<object_type_registry_entry_base> object_type() const {
    return object_type(std::type_index(typeid(Type)));
  }

  std::shared_ptr<object_type_registry_entry_base> object_type(const std::type_index &ti) const {
    return type_registry_.at(ti);
  }

  void clear() {
    type_registry_.clear();
  }

private:
  std::unordered_map<std::type_index, std::shared_ptr<object_type_registry_entry_base>> type_registry_;
};

}
}
#endif //MATADOR_OBJECT_TYPE_REGISTRY_HPP
