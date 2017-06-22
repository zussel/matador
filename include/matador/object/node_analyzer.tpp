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
template<class Owner, template < class U = Owner > class Observer >
void node_analyzer<Owner, Observer>::analyze()
{
  Owner obj;
  analyze(obj);
}

template<class Owner, template < class U = Owner > class Observer >
void node_analyzer<Owner, Observer>::analyze(Owner &obj)
{
  std::cout << node_.type() << " $$ analyzing " << typeid(Owner).name() << "\n";
  matador::access::serialize(*this, obj);
}

template<class Owner, template < class U = Owner > class Observer >
template<class Value>
void node_analyzer<Owner, Observer>::serialize(Value &x)
{
  matador::access::serialize(*this, x);
}

template<class Owner, template < class U = Owner > class Observer >
template<class Value>
void node_analyzer<Owner, Observer>::serialize(const char *id, belongs_to <Value> &x, cascade_type)
{
  std::cout << node_.type() << " $$ process belongs to '" << id << "' (type: " << x.type() << ")\n";

//  auto endpoint = std::make_shared<detail::relation_field_endpoint>(id, detail::relation_field_endpoint::BELONGS_TO, foreign_node.get());
  auto endpoint = std::make_shared<detail::belongs_to_endpoint<Value, Owner>>(id, &node_);

  node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

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

template<class Owner, template < class U = Owner > class Observer >
template<class Value>
void node_analyzer<Owner, Observer>::serialize(const char *id, has_one <Value> &x, cascade_type)
{
  std::cout << node_.type() << " $$ process has one '" << id << " (type: " << x.type() << ")\n";

  auto endpoint = std::make_shared<detail::has_one_endpoint<Value, Owner>>(id, &node_);

  node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

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

template<class Owner, template < class U = Owner > class Observer >
template<class Value, template<class ...> class Container>
void node_analyzer<Owner, Observer>::serialize(const char *id, has_many <Value, Container> &,
                                    const char *owner_column, const char *item_column,
                                    typename std::enable_if<!is_builtin<Value>::value>::type*)
{
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  std::cout << node_.type() << " $$ analyze: serializing " << id << "\n";
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    std::cout << node_.type() << " $$ has many relation " << id << " not found\n";


    // Value = foreign/owner type => left column
    // Owner = item/value type    => right column

    /*
     * Value = value type
     * Owner = owner type
     * new has_many_to_many<Value, Owner>
     */
    // handle observer
    std::vector<Observer<has_one_to_many_item<Value, Owner> >*> has_many_item_observer;
    for (auto o : observer_vector_) {
      has_many_item_observer.push_back(new Observer<has_one_to_many_item<Value, Owner> >(o));
    }

    auto endpoint = std::make_shared<detail::has_one_to_many_endpoint <Value, Owner>>(id, &node_);

//    node_.register_relation_endpoint(std::type_index(typeid(detail::has_one_to_many_endpoint <Value, Owner>)), endpoint);
    node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

    // new has many to many item
    auto proto = new has_one_to_many_item<Value, Owner>(owner_column, item_column);
    prototype_node *node = prototype_node::make_relation_node<has_one_to_many_item<Value, Owner> >(&store_, id, proto, false, node_.type(), id);

    std::cout << node_.type() << " $$ attach node " << id << " (typeindex:" << node->type_index().name() << ")\n";
    pi = store_.attach_internal<has_one_to_many_item<Value, Owner>>(node, nullptr, has_many_item_observer);

//    this->register_has_many_endpoint<Value>(node_, node_.type_index(), id, pi.get());

  } else {
    std::cout << node_.type() << " $$ found has many relation '" << id << "'\n";
    /*
     * switch left (Owner) and right (Value) template parameter
     * to fit the already inserted has_many_to_many_item
     * template parameter
     */

    /*
     * Value = value type
     * Owner = owner type
     * new has_many_to_many<Value, Owner>
     */
    std::type_index ti(typeid(has_one_to_many_item<Owner, Value>));
    if (pi->type_index() == ti) {

      prototype_iterator foreign_node = detach_one_to_many_node<Value>(pi);

      auto foreign_endpoint = std::make_shared<detail::has_many_to_many_endpoint <Value, Owner>>(id, foreign_node.get());

      std::cout << " foreign node type " << foreign_node->type() << "\n";
      foreign_node->register_relation_endpoint(std::type_index(typeid(Owner)), foreign_endpoint);
      std::vector<Observer<has_many_to_many_item<Value, Owner> >*> has_many_item_observer;
      for (auto o : observer_vector_) {
        has_many_item_observer.push_back(new Observer<has_many_to_many_item<Value, Owner> >(o));
      }

      std::cout << "Owner " << typeid(Owner).name() << "\n";
      std::cout << "Value " << typeid(Value).name() << "\n";
      auto endpoint = std::make_shared<detail::has_many_to_many_endpoint <Value, Owner>>(id, &node_);

      node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

      // link endpoints
      foreign_endpoint->foreign_endpoint = endpoint;
      endpoint->foreign_endpoint = foreign_endpoint;

      // new has many to many item
      auto proto = new has_many_to_many_item<Value, Owner>(owner_column, item_column);
      prototype_node *node = prototype_node::make_relation_node<has_many_to_many_item<Value, Owner>>(&store_, id, proto, false, node_.type(), id);

      std::cout << node_.type() << " $$ attach node " << id << " (typeindex:" << node->type_index().name() << ")\n";
      pi = store_.attach_internal<has_many_to_many_item<Value, Owner>>(node, nullptr, has_many_item_observer);

      std::cout << node_.type() << " $$ registering type id " << typeid(has_many_to_many_item<Value, Owner>).name() << " => " << pi->type() << "\n";
      store_.typeid_prototype_map_[typeid(has_many_to_many_item<Value, Owner>).name()].insert(std::make_pair(pi->type_, pi.get()));
    } else {

      // found corresponding belongs_to
      auto j = pi->find_endpoint(node_.type_index());
//      auto j = pi->find_endpoint(pi->type_index());
      if (j == pi->endpoint_end()) {
        // check for has many item
        throw_object_exception("prototype already inserted: " << pi->type());
      } else if (j->second->type == detail::basic_relation_endpoint::BELONGS_TO) {
        // set missing node
        auto endpoint = std::make_shared<detail::to_one_endpoint <Value, Owner>>(id, &node_);

        node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);
//        node_.register_relation_endpoint(std::type_index(typeid(detail::to_one_endpoint <Value, Owner>)), endpoint);

        j->second->foreign_endpoint = endpoint;
        endpoint->foreign_endpoint = j->second;
      }
    }
  }
}

template<class Owner, template < class U = Owner > class Observer >
template<class Value, template<class ...> class Container>
void node_analyzer<Owner, Observer>::serialize(const char *id, has_many <Value, Container> &,
                                    const char *owner_column, const char *item_column,
                                    typename std::enable_if<is_builtin<Value>::value>::type*)
{
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    std::vector<Observer<has_one_to_many_item<Value, Owner> >*> has_many_item_observer;
    for (auto o : observer_vector_) {
      has_many_item_observer.push_back(new Observer<has_one_to_many_item<Value, Owner> >(o));
    }

    auto proto = new has_one_to_many_item<Value, Owner>(owner_column, item_column);
    prototype_node *node = prototype_node::make_relation_node<has_one_to_many_item<Value, Owner> >(&store_, id, proto, false, node_.type(), id);

    std::cout << node_.type() << " $$ attach node " << id << " (typeindex:" << node->type_index().name() << ")\n";
    pi = store_.attach_internal<has_one_to_many_item<Value, Owner> >(node, nullptr, has_many_item_observer);
  } else {
    // throw exception
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

template<class Owner, template < class U = Owner > class Observer >
template<class Value>
prototype_iterator node_analyzer<Owner, Observer>::detach_one_to_many_node(prototype_iterator node)
{
  // found has one to many
  // must be detached because
  // we have a many to many relation here
  std::cout << node_.type() << " $$ detaching node " << node->type() << "\n";
  store_.detach(node);
  // remove registered endpoint from foreign site
  prototype_iterator foreign_node = store_.find<Value>();
  if (foreign_node != store_.end()) {
    foreign_node->unregister_relation_endpoint(node_.type_index());
  }
  std::cout << "foreign node endpoints size " << foreign_node->endpoints_size() << "\n";
  return foreign_node;
}

}
}