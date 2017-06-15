#include "matador/object/node_analyzer.hpp"
#include "matador/object/prototype_iterator.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/has_many_item_holder.hpp"
#include "matador/object/generic_access.hpp"
#include "matador/object/to_many_endpoints.hpp"

namespace matador {
namespace detail {

/*
 * no observer version
 */
template<class T, template < class U = T > class O >
void node_analyzer<T, O>::analyze()
{
  T obj;
  analyze(obj);
}

template<class T, template < class U = T > class O >
void node_analyzer<T, O>::analyze(T &obj)
{
  std::cout << node_.type() << " $$ analyzing " << typeid(T).name() << "\n";
  matador::access::serialize(*this, obj);
}

template<class T, template < class U = T > class O >
template<class V>
void node_analyzer<T, O>::serialize(V &x)
{
  matador::access::serialize(*this, x);
}

template<class T, template < class U = T > class O >
template<class V>
void node_analyzer<T, O>::serialize(const char *id, belongs_to <V> &x, cascade_type)
{
  std::cout << node_.type() << " $$ process belongs to '" << id << "' (type: " << x.type() << ")\n";

//  auto endpoint = std::make_shared<detail::relation_field_endpoint>(id, detail::relation_field_endpoint::BELONGS_TO, foreign_node.get());
  auto endpoint = std::make_shared<detail::belongs_to_endpoint<T, V>>(id, &node_);

  node_.register_relation_endpoint(std::type_index(typeid(V)), endpoint);

  // find foreign_node of belongs to type
  prototype_iterator foreign_node = store_.find(x.type());
  if (foreign_node != store_.end()) {
    /*
     * foreign_node is already inserted
     * check if foreign_node was created from has_many
     * check if foreign_node has_many relation for id (id == tablename)
     */
    auto i = foreign_node->find_endpoint(node_.type_index());
    if (i != foreign_node->endpoint_end()) {


      if (i->second->type == detail::basic_relation_endpoint::HAS_MANY) {
        // yes, foreign_node was created from has_many!
        // detach current foreign_node (has_many_item == relation table)
        if (i->second->node != nullptr) {
          std::cout << node_.type() << " $$ detaching foreign_node " << i->second->node->type() << "\n";
          store_.detach(i->second->node);
        }
        i->second->node = nullptr;
      } else if (i->second->type == detail::basic_relation_endpoint::HAS_ONE) {
        // foreign_node was created from has_one
        // check if foreign_node is set
        if (i->second->node == nullptr) {
          i->second->foreign_endpoint = endpoint;
          i->second->node = &node_;

        }
      }
      endpoint->foreign_endpoint = i->second;
      i->second->foreign_endpoint = endpoint;
    }
  }
}

template<class T, template < class U = T > class O >
template<class V>
void node_analyzer<T, O>::serialize(const char *id, has_one <V> &x, cascade_type)
{
  std::cout << node_.type() << " $$ process has one '" << id << " (type: " << x.type() << ")\n";

  auto endpoint = std::make_shared<detail::has_one_endpoint<T, V>>(id, &node_);

  node_.register_relation_endpoint(std::type_index(typeid(V)), endpoint);

  prototype_iterator foreign_node = store_.find(x.type());

  if (foreign_node != store_.end()) {
    auto i = foreign_node->find_endpoint(node_.type_index());
    if (i != foreign_node->endpoint_end()) {
      if (i->second->type == basic_relation_endpoint::BELONGS_TO) {
        // link both nodes
        endpoint->foreign_endpoint = i->second;
        i->second->foreign_endpoint = endpoint;
      } else {
        throw object_exception("invalid endpoint type");
      }
    }
  }
}

template<class T, template < class U = T > class O >
template<class V, template<class ...> class C>
void node_analyzer<T, O>::serialize(const char *id, has_many <V, C> &,
                                    const char *owner_column, const char *item_column,
                                    typename std::enable_if<!is_builtin<V>::value>::type*)
{
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  std::cout << node_.type() << " $$ analyze: serializing " << id << "\n";
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    std::cout << node_.type() << " $$ has many relation " << id << " not found\n";


    // V = foreign/owner type => left column
    // T = item/value type    => right column

    /*
     * V = value type
     * T = owner type
     * new has_many_to_many<T, V>
     */
    // handle observer
    std::vector<O<has_one_to_many_item<V, T> >*> has_many_item_observer;
    for (auto o : observer_vector_) {
      has_many_item_observer.push_back(new O<has_one_to_many_item<V, T> >(o));
    }

    auto endpoint = std::make_shared<detail::has_one_to_many_endpoint <V, T>>(id, &node_);

    node_.register_relation_endpoint(std::type_index(typeid(detail::has_one_to_many_endpoint <V, T>)), endpoint);

    // new has many to many item
    auto proto = new has_one_to_many_item<V, T>(owner_column, item_column);
    prototype_node *node = prototype_node::make_relation_node<has_one_to_many_item<V, T> >(&store_, id, proto, false, node_.type(), id);

    std::cout << node_.type() << " $$ attach node " << id << " (typeindex:" << node->type_index().name() << ")\n";
    pi = store_.attach_internal<has_one_to_many_item<V, T>>(node, nullptr, has_many_item_observer);

//    this->register_has_many_endpoint<V>(node_, node_.type_index(), id, pi.get());

  } else {
    std::cout << node_.type() << " $$ found has many relation '" << id << "'\n";
    /*
     * switch left (T) and right (V) template parameter
     * to fit the already inserted has_many_to_many_item
     * template parameter
     */

    /*
     * V = value type
     * T = owner type
     * new has_many_to_many<V, T>
     */
    std::type_index ti(typeid(has_one_to_many_item<T, V>));
    if (pi->type_index() == ti) {

      prototype_iterator foreign_node = detach_one_to_many_node<V>(pi);

      std::vector<O<has_many_to_many_item<T, V> >*> has_many_item_observer;
      for (auto o : observer_vector_) {
        has_many_item_observer.push_back(new O<has_many_to_many_item<T, V> >(o));
      }

      auto endpoint = std::make_shared<detail::has_many_to_many_endpoint <T, V>>(id, &node_);

      node_.register_relation_endpoint(std::type_index(typeid(detail::has_many_to_many_endpoint <T, V>)), endpoint);

      // new has many to many item
      auto proto = new has_many_to_many_item<T, V>(owner_column, item_column);
      prototype_node *node = prototype_node::make_relation_node<has_many_to_many_item<T, V>>(&store_, id, proto, false, node_.type(), id);

      std::cout << node_.type() << " $$ attach node " << id << " (typeindex:" << node->type_index().name() << ")\n";
      pi = store_.attach_internal<has_many_to_many_item<T, V>>(node, nullptr, has_many_item_observer);

      store_.typeid_prototype_map_[typeid(has_many_to_many_item<V, T>).name()].insert(std::make_pair(pi->type_, pi.get()));
    } else {

      // found corresponding belongs_to
      auto j = pi->find_endpoint(node_.type_index());
//      auto j = pi->find_endpoint(pi->type_index());
      if (j == pi->endpoint_end()) {
        // check for has many item
        throw_object_exception("prototype already inserted: " << pi->type());
      } else if (j->second->type == detail::basic_relation_endpoint::BELONGS_TO) {
        // set missing node
        auto endpoint = std::make_shared<detail::to_one_endpoint <V, T>>(id, &node_);

        node_.register_relation_endpoint(std::type_index(typeid(detail::to_one_endpoint <V, T>)), endpoint);

        j->second->foreign_endpoint = endpoint;
        endpoint->foreign_endpoint = j->second;
      }
    }
  }
}

template<class T, template < class U = T > class O >
template<class V, template<class ...> class C>
void node_analyzer<T, O>::serialize(const char *id, has_many <V, C> &,
                                    const char *owner_column, const char *item_column,
                                    typename std::enable_if<is_builtin<V>::value>::type*)
{
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    std::vector<O<has_one_to_many_item<V, T> >*> has_many_item_observer;
    for (auto o : observer_vector_) {
      has_many_item_observer.push_back(new O<has_one_to_many_item<V, T> >(o));
    }

    auto proto = new has_one_to_many_item<V, T>(owner_column, item_column);
    prototype_node *node = prototype_node::make_relation_node<has_one_to_many_item<V, T> >(&store_, id, proto, false, node_.type(), id);

    std::cout << node_.type() << " $$ attach node " << id << " (typeindex:" << node->type_index().name() << ")\n";
    pi = store_.attach_internal<has_one_to_many_item<V, T> >(node, nullptr, has_many_item_observer);
  } else {
    // throw exception
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

template<class T, template < class U = T > class O >
template<class V>
prototype_iterator node_analyzer<T, O>::detach_one_to_many_node(prototype_iterator node)
{
  // found has one to many
  // must be detached because
  // we have a many to many relation here
  std::cout << node_.type() << " $$ detaching node " << node->type() << "\n";
  store_.detach(node);
  // remove registered endpoint from foreign site
  prototype_iterator foreign_node = store_.find<V>();
  if (foreign_node != store_.end()) {
//    std::cout << node_.type() << " $$ node [" << foreign_node->type() << "] unregister endpoint node::field[" << id << " (typeindex: " << foreign_node->type_index().name() << "]\n";
    foreign_node->unregister_relation_endpoint(foreign_node->type_index());
  }
  return foreign_node;
}

}
}