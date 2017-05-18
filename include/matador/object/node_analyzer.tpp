#include "matador/object/node_analyzer.hpp"
#include "matador/object/prototype_iterator.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/generic_access.hpp"
#include "matador/object/has_many_to_many_item.hpp"
#include "matador/object/has_one_to_many_item.hpp"

namespace matador {
namespace detail {

/*
 * no observer version
 */
template<class T, template < class U = T > class O >
void node_analyzer<T, O>::analyze()
{
  T obj;
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
  this->process_belongs_to<V>(id, x);
}

template<class T, template < class U = T > class O >
template<class V>
void node_analyzer<T, O>::serialize(const char *id, has_one <V> &x, cascade_type)
{
  this->process_has_one<V>(id, x);
}

template<class T, template < class U = T > class O >
template<class V, template<class ...> class C>
void node_analyzer<T, O>::serialize(const char *id, has_many <V, C> &x,
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

    std::vector<O<has_one_to_many_item<V, T> >*> has_many_item_observer;
    for (auto o : observer_vector_) {
      has_many_item_observer.push_back(new O<has_one_to_many_item<V, T> >(o));
    }
    // new has many to many item
    prototype_node *node = prototype_node::make_relation_node<has_one_to_many_item<V, T>>(&store_, id, false, node_.type(), id, owner_column, item_column);

    pi = store_.attach_internal<has_one_to_many_item<V, T>>(node, nullptr, has_many_item_observer);
    std::cout << node_.type() << " $$ attached node " << id << " (typeindex:" << pi->type_index().name() << ")\n";

    this->register_has_many_endpoint<V>(node_.type_index(), id, pi.get());

  } else {
    std::cout << node_.type() << " $$ found has many relation " << id << "\n";
    this->process_has_many<V, C>(pi, id, x, owner_column, item_column);
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


    prototype_node *node = prototype_node::make_relation_node<has_one_to_many_item<V, T> >(
      &store_, id, false, node_.type(), id, owner_column, item_column
    );

    pi = store_.attach_internal<has_one_to_many_item<V, T> >(node, nullptr, has_many_item_observer);
    std::cout << node_.type() << " $$ attached node " << id << " (typeindex:" << pi->type_index().name() << ")\n";
  } else {
    // throw exception
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

template<class T, template < class U = T > class O >
template<class V >
void node_analyzer<T, O>::process_belongs_to(const char *id, belongs_to <V> &x)
{
  std::cout << node_.type() << " $$ process belongs to '" << id << " (type: " << x.type() << ")\n";
  // find node of belongs to type
  prototype_iterator node = store_.find(x.type());

  auto endpoint = std::make_shared<detail::relation_field_endpoint>(id, detail::relation_field_endpoint::BELONGS_TO, node.get());

  if (node != store_.end()) {
    /*
     * node is already inserted
     * check if node was created from has_many
     * check if node has_many relation for id (id == tablename)
     */
    auto i = node->relation_field_endpoint_map_.find(node->type_index());
    if (i != node->relation_field_endpoint_map_.end()) {
      if (i->second->type == detail::relation_field_endpoint::HAS_MANY) {
        // yes, node was created from has_many!
        // detach current node (has_many_item == relation table)
        if (i->second->node != nullptr) {
          std::cout << node_.type() << " $$ detaching node " << i->second->node->type() << "\n";
          store_.detach(i->second->node);
        }
        i->second->node = nullptr;
      } else if (i->second->type == detail::relation_field_endpoint::HAS_ONE) {
        // node was created from has_one
        // check if node is set
        if (i->second->node == nullptr) {
          i->second->foreign_endpoint = endpoint;
          i->second->node = &node_;

        }
      }
      endpoint->foreign_endpoint = i->second;
      i->second->foreign_endpoint = endpoint;
    }
  }

  {
    auto fe = endpoint->foreign_endpoint.lock();
    std::cout << node_.type() << " $$ node::field[" << id << " (typeindex: " << node_.type_index().name() << "]";
    if (fe) {
      std::cout << ": foreign endpoint -> " << fe->field;
    }
    std::cout << "\n";
  }
  node_.register_relation_field_endpoint(node_.type_index(), endpoint);
}

template<class T, template < class U = T > class O >
template< class V >
void node_analyzer<T, O>::process_has_one(const char *id, has_one <V> &x)
{
  std::cout << node_.type() << " $$ process has one '" << id << " (type: " << x.type() << ")\n";
  prototype_iterator foreign_node = store_.find(x.type());

  auto endpoint = std::make_shared<detail::relation_field_endpoint>(id, detail::relation_field_endpoint::HAS_ONE, foreign_node.get());

  if (foreign_node != store_.end()) {
    auto i = foreign_node->relation_field_endpoint_map_.find(foreign_node->type_index());
    if (i != foreign_node->relation_field_endpoint_map_.end()) {
      endpoint->foreign_endpoint = i->second;
      i->second->foreign_endpoint = endpoint;
    }
  }

  {
    auto fe = endpoint->foreign_endpoint.lock();
    std::cout << node_.type() << " $$ node::field[" << id << " (typeindex: " << node_.type_index().name() << "]";
    if (fe) {
      std::cout << ": foreign endpoint -> " << fe->field;
    }
    std::cout << "\n";
  }
  node_.register_relation_field_endpoint(node_.type_index(), endpoint);
}

template<class T, template < class U = T > class O >
template<class V, template<class ...> class C>
void node_analyzer<T, O>::process_has_many(const prototype_iterator &pi, const char *id, has_many<V, C> &, const char *owner_column, const char *item_column)
{
  std::cout << node_.type() << " $$ process has many '" << id << "\n";
  /*
   * switch left (T) and right (V) template parameter
   * to fit the already inserted has_many_to_many_item
   * template parameter
   */
  std::type_index ti(typeid(has_one_to_many_item<T, V>));
  if (pi->type_index() == ti) {
    // found has one to many
    // must be detached because
    // we have a many to many relation here
    std::cout << node_.type() << " $$ detaching node " << pi->type() << "\n";
    store_.detach(pi);
    // remove registered endpoint from foreign site
    prototype_iterator fp = store_.find<V>();
    if (fp != store_.end()) {
      std::cout << node_.type() << " $$ node [" << fp->type() << "] unregister endpoint node::field[" << id << " (typeindex: " << fp->type_index().name() << "]\n";
      fp->unregister_relation_field_endpoint(fp->type_index());
    }

    std::vector<O<has_many_to_many_item<T, V> >*> has_many_item_observer;
    for (auto o : observer_vector_) {
      has_many_item_observer.push_back(new O<has_many_to_many_item<T, V> >(o));
    }
    // new has many to many item
    prototype_node *node = prototype_node::make_relation_node<has_many_to_many_item<T, V>>(&store_, id, false, node_.type(), id, owner_column, item_column);

    auto pi2 = store_.attach_internal<has_many_to_many_item<T, V>>(node, nullptr, has_many_item_observer);
    std::cout << node_.type() << " $$ attached node " << id << " (typeindex:" << pi2->type_index().name() << ")\n";

    register_has_many_endpoint<V>(node_.type_index(), id, pi2.get());

    // prototype is of type has_many_to_many_item
    register_has_many_endpoint<T>(node_.type_index(), id, pi.get());
    store_.typeid_prototype_map_[ti.name()].insert(std::make_pair(pi->type_, pi.get()));
  } else {
    // found corresponding belongs_to or has_many
    auto j = pi->relation_field_endpoint_map_.find(pi->type_index());
    if (j == pi->relation_field_endpoint_map_.end()) {
      // check for has many item
      throw_object_exception("prototype already inserted: " << pi->type());
    } else if (j->second->type == detail::relation_field_endpoint::BELONGS_TO) {
      // set missing node
      j->second->node = &node_;
      j->second->foreign_endpoint = j->second;
      this->register_has_many_endpoint<V>(pi->type_index(), id, pi.get());
    }
  }
}

template<class T, template < class U = T > class O >
template<class V>
void node_analyzer<T, O>::register_has_many_endpoint(const std::type_index &typeindex, const char *id,
                                                     prototype_node *node)
{
  auto endpoint = std::make_shared<detail::relation_field_endpoint>(id, detail::relation_field_endpoint::HAS_MANY, node);

  // find counterpart node
  prototype_iterator foreign_node = store_.find<V>();
  if (foreign_node != store_.end()) {
    auto i = foreign_node->relation_field_endpoint_map_.find(foreign_node->type_index());
    if (i != foreign_node->relation_field_endpoint_map_.end()) {
      /*
       * found matching endpoint
       */
      endpoint->foreign_endpoint = i->second;
      endpoint->side =relation_field_endpoint::RIGHT;
      i->second->foreign_endpoint = endpoint;
//      std::cout << "basic_node_analyzer::register_has_many_endpoint: endpoint " << endpoint->field << "(" << endpoint.get() << ") -> foreign " << i->second->field << "\n";
//      std::cout << "basic_node_analyzer::register_has_many_endpoint: endpoint " << i->second->field << "(" << i->second.get() << ") -> foreign " << endpoint->field << "\n";
    }
  }

  {
    auto fe = endpoint->foreign_endpoint.lock();
    std::cout << node_.type() << " $$ node::field[" << id << " (typeindex: " << typeindex.name() << "]";
    if (fe) {
      std::cout << ": foreign endpoint -> " << fe->field;
    }
    std::cout << "\n";
  }
  node_.register_relation_field_endpoint(typeindex, endpoint);
}

}
}