#include "oos/object/node_analyzer.hpp"
#include "oos/object/prototype_iterator.hpp"
#include "oos/object/object_store.hpp"
#include "oos/object/generic_access.hpp"

namespace oos {
namespace detail {


template<class T>
void node_analyzer<T>::analyze()
{
//  std::cout << "START analyzing " << node_.type() << "\n";
  T obj;
  oos::access::serialize(*this, obj);
//  std::cout << "FINISHED analyzing " << node_.type() << "\n";
}

template<class T>
template<class V>
void node_analyzer<T>::serialize(V &x)
{
  oos::access::serialize(*this, x);
}

template<class T>
template<class V>
void node_analyzer<T>::serialize(const char *id, belongs_to <V> &x, cascade_type)
{
//  std::cout << "analyzing belongs_to field " << id << " (typeid: " << typeid(V).name() << ")\n";
  prototype_iterator node = node_.tree()->find(x.type());
  if (node != node_.tree()->end()) {
    // check if created from has_many
    // check if node has_many relation for id (id == tablename)
    auto i = node->has_many_map_.find(node->type_index());
    if (i != node->has_many_map_.end()) {
      // yes, node found!
      // Todo: check if node is for has many item
      // detach has_many_item node
      node_.tree()->detach(i->second.node);
      i->second.node = nullptr;
    }
  } else {
    node = node_.tree()->template prepare_attach<V>();
  }
  if (!node->has_primary_key()) {
    throw_object_exception("serializable of type '" << x.type() << "' has no primary key");
  }
  node_.register_belongs_to(std::type_index(typeid(V)), prototype_node::relation_info(id, [](void *obj, const std::string &field, oos::object_proxy *owner) {
    oos::set(static_cast<T*>(obj), field, object_ptr<V>(owner));
  }, [](void *obj, const std::string &field, oos::object_proxy *) {
    oos::set(static_cast<T*>(obj), field, object_ptr<V>());
  }, node.get()));
}

template<class T>
template<class V>
void node_analyzer<T>::serialize(const char *, has_one <V> &x, cascade_type)
{
//  std::cout << "analyzing has_one field " << id << " (typeid: " << typeid(V).name() << ")\n";
  prototype_iterator node = node_.tree()->find(x.type());
  if (node == node_.tree()->end()) {
    // if there is no such prototype node
    // prepare insertion of new node
    node = node_.tree()->template prepare_attach<V>();
  } else if (!node->has_primary_key()) {
    throw_object_exception("serializable of type '" << x.type() << "' has no primary key");
  }
}

template<class T>
template<class V, template<class ...> class C>
void node_analyzer<T>::serialize(const char *id, has_many <V, C> &, const char *, const char *,
                                            typename std::enable_if<!std::is_scalar<V>::value>::type*)
//void node_analyzer<T>::serialize(const char *id, has_many<V, C> &x, const char */*owner_field*/, const char */*item_field*/)
{
//  std::cout << "analyzing has_many field " << id << " (typeid: " << typeid(V).name() << ")\n";
  // item column column names
//  x.owner_field(owner_field);
//  x.item_field(item_field);
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = node_.tree()->find(id);
  if (pi == node_.tree()->end()) {
    pi = node_.tree()->template attach<typename has_many<V, C>::item_type>(id, false, nullptr);
    node_.register_has_many(node_.type_index(), prototype_node::relation_info(id, [](void *obj, const std::string &field, oos::object_proxy *owner) {
      oos::append(static_cast<T*>(obj), field, object_ptr<V>(owner));
    }, [](void *obj, const std::string &field, oos::object_proxy *owner) {
      oos::remove(static_cast<T*>(obj), field, object_ptr<V>(owner));
    }, pi.get()));
  } else if (pi->type_index() == std::type_index(typeid(typename has_many<V, C>::item_type))) {
    // prototype is of type has_many_item
    throw_object_exception("many to many relations are not supported by now");
  } else {
    // throw exception
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

template<class T>
template<class V, template<class ...> class C>
void node_analyzer<T>::serialize(const char *id, has_many <V, C> &, const char *, const char *,
                                            typename std::enable_if<std::is_scalar<V>::value>::type*)
//void node_analyzer<T>::serialize(const char *id, has_many<V, C> &x, const char */*owner_field*/, const char */*item_field*/)
{
  // item column column names
//  x.owner_field(owner_field);
//  x.item_field(item_field);
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = node_.tree()->find(id);
  if (pi == node_.tree()->end()) {
    pi = node_.tree()->template attach<typename has_many<V, C>::item_type>(id, false, nullptr);
  } else if (pi->type_index() == std::type_index(typeid(typename has_many<V, C>::item_type))) {
    // prototype is of type has_many_item
    throw_object_exception("many to many relations are not supported by now");
  } else {
    // throw exception
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

}
}