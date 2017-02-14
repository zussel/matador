#include "oos/object/node_analyzer.hpp"
#include "oos/object/prototype_iterator.hpp"
#include "oos/object/object_store.hpp"
#include "oos/object/generic_access.hpp"

namespace oos {
namespace detail {

template<class V, class T>
void basic_node_analyzer::process_belongs_to(const char *id, belongs_to <V> &x)
{
  prototype_iterator node = store_.find(x.type());
  if (node != store_.end()) {
    // check if created from has_many
    // check if node has_many relation for id (id == tablename)
    auto i = node->relation_info_map_.find(node->type_index());
    if (i != node->relation_info_map_.end() && i->second.type == prototype_node::relation_info::HAS_MANY) {
      // yes, node found!
      // Todo: check if node is for has many item
      // detach has_many_item node
      if (i->second.node != nullptr) {
        store_.detach(i->second.node);
      }
      i->second.node = nullptr;
    }
  }
  object_store &store = store_;
  node_.register_belongs_to(std::type_index(typeid(V)),
                            prototype_node::relation_info(id,
                                                          prototype_node::relation_info::BELONGS_TO,
                                                          [&store](object_proxy *proxy, const std::string &field, oos::object_proxy *owner) {
    store.mark_modified<T>(proxy);
    oos::set(proxy->obj<T>(), field, object_ptr<V>(owner));
  }, [&store](object_proxy *proxy, const std::string &field, oos::object_proxy *) {
    store.mark_modified<T>(proxy);
    oos::set(proxy->obj<T>(), field, object_ptr<V>());
  }, node.get()));
}

template<class V, class T>
void basic_node_analyzer::process_has_one(const char *, has_one <V> &)
{
//  std::cout << "analyzing has_one field " << id << " (typeid: " << typeid(V).name() << ")\n";
//  prototype_iterator node = node_.tree()->find(x.type());
//  if (node == node_.tree()->end()) {
//    // if there is no such prototype node
//    // prepare insertion of new node
//    node = node_.tree()->template prepare_attach<V>();
//  } else if (!node->has_primary_key()) {
//    throw_object_exception("serializable of type '" << x.type() << "' has no primary key");
//  }
}

template<class V, class T, template<class ...> class C>
void basic_node_analyzer::process_has_many(const prototype_iterator &pi, const char *id, has_many<V, C> &)
{
  if (pi->type_index() == std::type_index(typeid(typename has_many<T, C>::item_type))) {
    // prototype is of type has_many_item
    std::cout << "serialize has many: found type " << pi->type_index().name() << "\n";
    this->register_has_many<V, T>(node_.type_index(), id, pi.get());
    std::cout << "registering node type id " << typeid(typename has_many<V, C>::item_type).name() << "\n";
    store_.typeid_prototype_map_[typeid(typename has_many<V, C>::item_type).name()].insert(std::make_pair(pi->type_, pi.get()));
  } else {
    // found corresponding belongs_to
    auto j = pi->relation_info_map_.find(node_.type_index_);
    if (j == pi->relation_info_map_.end()) {
      // check for has many item
      throw_object_exception("prototype already inserted: " << pi->type());
    } else if (j->second.type == prototype_node::relation_info::BELONGS_TO) {
      // set missing node
      j->second.node = &node_;
      this->register_has_many<V, T>(pi->type_index(), id, pi.get());
    } else if (j->second.type == prototype_node::relation_info::HAS_MANY) {
      // handle has many
    }
  }
}

template<class V, class T>
void basic_node_analyzer::register_has_many(const std::type_index &typeindex, const char *id, prototype_node *node)
{
  object_store &store = store_;
  node_.register_has_many(typeindex,
                          prototype_node::relation_info(id,
                                                        prototype_node::relation_info::HAS_MANY,
                                                        [&store](object_proxy *proxy, const std::string &field, oos::object_proxy *owner) {
    store.mark_modified<T>(proxy);
    oos::append(proxy->obj<T>(), field, object_ptr<V>(owner));
  }, [&store](object_proxy *proxy, const std::string &field, oos::object_proxy *owner) {
    store.mark_modified<T>(proxy);
    oos::remove(proxy->obj<T>(), field, object_ptr<V>(owner));
  }, node));
}

template<class T, template < class U = T > class O>
void node_analyzer<T, O>::analyze()
{
  T obj;
  oos::access::serialize(*this, obj);
}

template<class T, template < class U = T > class O>
template<class V>
void node_analyzer<T, O>::serialize(V &x)
{
  oos::access::serialize(*this, x);
}

template<class T, template < class U = T > class O>
template<class V>
void node_analyzer<T, O>::serialize(const char *id, belongs_to <V> &x, cascade_type)
{
  this->process_belongs_to<V, T>(id, x);
}

template<class T, template < class U = T > class O>
template<class V>
void node_analyzer<T, O>::serialize(const char *id, has_one <V> &x, cascade_type)
{
  this->process_has_one<V, T>(id, x);
}

template<class T, template < class U = T > class O>
template<class V, template<class ...> class C>
void node_analyzer<T, O>::serialize(const char *id, has_many <V, C> &x, const char *owner_column, const char *item_column,
                                            typename std::enable_if<!std::is_scalar<V>::value>::type*)
{
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    std::vector<O<typename has_many<V, C>::item_type>*> has_many_item_observer;
    for (auto o : observer_) {
      has_many_item_observer.push_back(new O<typename has_many<V, C>::item_type>(o));
    }

    prototype_node *node = prototype_node::make_relation_node<typename has_many<V, C>::item_type>(&store_, id, false, node_.type(), id, owner_column, item_column);

    pi = store_.attach<typename has_many<V, C>::item_type>(node, nullptr, has_many_item_observer);

    this->register_has_many<V, T>(pi->type_index(), id, pi.get());
  } else {
    this->process_has_many<V, T, C>(pi, id, x);
  }
//  if (pi->type_index() == std::type_index(typeid(typename has_many<V, C>::item_type))) {
//    // prototype is of type has_many_item
//    throw_object_exception("many to many relations are not supported by now");
//  } else if (pi->type_index() == std::type_index(typeid(typename has_many<T, C>::item_type))) {
//    std::cout << "serialize has many: found type " << pi->type_index().name() << "\n";
//    this->register_has_many<V, T>(std::type_index(typeid(typename has_many<T, C>::item_type)), id, pi.get());
//  } else {
//    // found corresponding belongs_to
//    auto j = pi->relation_info_map_.find(node_.type_index_);
//    if (j == pi->relation_info_map_.end()) {
//      throw_object_exception("prototype already inserted: " << pi->type());
//    } else if (j->second.type == prototype_node::relation_info::BELONGS_TO) {
//      // set missing node
//      j->second.node = &node_;
//      this->register_has_many<V, T>(pi->type_index(), id, pi.get());
//    } else if (j->second.type == prototype_node::relation_info::HAS_MANY) {
//      // handle has many
//    }
//  }
}

template<class T, template < class U = T > class O>
template<class V, template<class ...> class C>
void node_analyzer<T, O>::serialize(const char *id, has_many <V, C> &, const char *owner_column, const char *item_column,
                                            typename std::enable_if<std::is_scalar<V>::value>::type*)
{
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    std::vector<O<typename has_many<V, C>::item_type>*> has_many_item_observer;
    for (auto o : observer_) {
      has_many_item_observer.push_back(new O<typename has_many<V, C>::item_type>(o));
    }

    prototype_node *node = prototype_node::make_relation_node<typename has_many<V, C>::item_type>(&store_, id, false, node_.type(), id, owner_column, item_column);

    pi = store_.attach<typename has_many<V, C>::item_type>(node, nullptr, has_many_item_observer);
  } else {
    // throw exception
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

/*
 * no observer version
 */
template<class T>
void node_analyzer<T>::analyze()
{
  T obj;
  oos::access::serialize(*this, obj);
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
  this->process_belongs_to<V, T>(id, x);
}

template<class T>
template<class V>
void node_analyzer<T>::serialize(const char *id, has_one <V> &x, cascade_type)
{
  this->process_has_one<V, T>(id, x);
}

template<class T>
template<class V, template<class ...> class C>
void node_analyzer<T>::serialize(const char *id, has_many <V, C> &x, const char *owner_column, const char *item_column,
                                 typename std::enable_if<!std::is_scalar<V>::value>::type*)
{
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    prototype_node *node = prototype_node::make_relation_node<typename has_many<V, C>::item_type>(&store_, id, false, node_.type(), id, owner_column, item_column);

    pi = store_.attach<typename has_many<V, C>::item_type>(node, nullptr);

    this->register_has_many<V, T>(node_.type_index(), id, pi.get());
  } else {
    this->process_has_many<V, T, C>(pi, id, x);
  }
//  if (pi->type_index() == std::type_index(typeid(typename has_many<V, C>::item_type))) {
//    // prototype is of type has_many_item
//    throw_object_exception("prototype already inserted");
//  } else {
//    if (pi->type_index() == std::type_index(typeid(typename has_many<T, C>::item_type))) {
//      // prototype is of type has_many_item
//      std::cout << "serialize has many: found type " << pi->type_index().name() << "\n";
//      this->register_has_many<V, T>(node_.type_index(), id, pi.get());
//      std::cout << "registering node type id " << typeid(typename has_many<V, C>::item_type).name() << "\n";
//      store_.typeid_prototype_map_[typeid(typename has_many<V, C>::item_type).name()].insert(std::make_pair(pi->type_, pi.get()));
//    } else {
//      // found corresponding belongs_to
//      auto j = pi->relation_info_map_.find(node_.type_index_);
//      if (j == pi->relation_info_map_.end()) {
//        // check for has many item
//        throw_object_exception("prototype already inserted: " << pi->type());
//      } else if (j->second.type == prototype_node::relation_info::BELONGS_TO) {
//        // set missing node
//        j->second.node = &node_;
//        this->register_has_many<V, T>(pi->type_index(), id, pi.get());
//      } else if (j->second.type == prototype_node::relation_info::HAS_MANY) {
//        // handle has many
//      }
//    }
//  }
}

template<class T>
template<class V, template<class ...> class C>
void node_analyzer<T>::serialize(const char *id, has_many <V, C> &, const char *owner_column, const char *item_column,
                                 typename std::enable_if<std::is_scalar<V>::value>::type*)
{
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    prototype_node *node = prototype_node::make_relation_node<typename has_many<V, C>::item_type>(&store_, id, false, node_.type(), id, owner_column, item_column);

    pi = store_.attach<typename has_many<V, C>::item_type>(node, nullptr);
  } else {
    // throw exception
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

}
}