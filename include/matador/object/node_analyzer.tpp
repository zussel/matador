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
//  std::cout << "ANALYZING " << typeid(Owner).name() << "\n";
  analyze(obj);
}

template<class Owner, template < class U = Owner > class Observer >
void node_analyzer<Owner, Observer>::analyze(Owner &obj)
{
//  std::cout << "ANALYZING " << typeid(Owner).name() << "\n";
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
//  std::cout << "ANALYZING " << typeid(Owner).name() << " found BELONGS_TO<" << typeid(Value).name() << ">\n";
//  std::cout << "ANALYZING " << typeid(Owner).name() << " searching foreign node <" << x.type() << ">\n";
  // find foreign_node of belongs to type
  prototype_iterator foreign_node = store_.find(x.type());
  if (foreign_node != store_.end()) {
    /*
     * foreign_node is already inserted
     * check if foreign_node was created from has_many
     * check if foreign_node has_many relation for id (id == tablename)
     */
//    std::cout << "ANALYZING " << typeid(Owner).name() << " searching foreign endpoint<" << node_.type_index().name() << ">\n";

    auto i = foreign_node->find_endpoint(node_.type_index());

    auto eps = foreign_node->endpoints();

    if (i != foreign_node->endpoint_end()) {

      if (i->second->type == detail::basic_relation_endpoint::HAS_MANY) {
        // yes, foreign_node was created from has_many!
        // detach current foreign_node (has_many_item == relation table)
        store_.detach(node_.type());

//        std::cout << "serialize BELONGS_TO $$ creating detail::many_to_one_endpoint<Value: " << typeid(Value).name() << ", Owner: " << typeid(Owner).name() << ">\n";
        auto foreign_endpoint = std::make_shared<detail::many_to_one_endpoint<Owner, Value>>(i->second->field, foreign_node.get());
        foreign_node->unregister_relation_endpoint(node_.type_index());
        foreign_node->register_relation_endpoint(node_.type_index(), foreign_endpoint);

        auto endpoint = std::make_shared<detail::belongs_to_many_endpoint<Value, Owner>>(id, &node_);
        node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

        endpoint->foreign_endpoint = foreign_endpoint;
        foreign_endpoint->foreign_endpoint = endpoint;
      } else if (i->second->type == detail::basic_relation_endpoint::HAS_ONE) {
        // foreign_node was created from has_one
        // check if foreign_node is set
        auto endpoint = std::make_shared<detail::belongs_to_one_endpoint<Value, Owner>>(id, &node_);

        node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);
        endpoint->foreign_endpoint = i->second;
        i->second->foreign_endpoint = endpoint;
      } else {
        throw_object_exception("foreign node is already a belongs to endpoint (node: " << node_.type() << ", field: " << id << ")");
      }
    } else {
      auto endpoint = std::make_shared<detail::belongs_to_one_endpoint<Value, Owner>>(id, &node_);

      node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);
    }
  } else {
    auto endpoint = std::make_shared<detail::belongs_to_one_endpoint<Value, Owner>>(id, &node_);

    node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);
  }
}

template<class Owner, template < class U = Owner > class Observer >
template<class Value>
void node_analyzer<Owner, Observer>::serialize(const char *id, has_one <Value> &x, cascade_type)
{
//  std::cout << "ANALYZING " << typeid(Owner).name() << " found HAS_ONE<" << typeid(Value).name() << ">\n";

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
//  std::cout << "ANALYZING " << typeid(Owner).name() << " found HAS_MANY<" << typeid(Value).name() << ">\n";
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    // Value = foreign/owner type => left column
    // Owner = item/value type    => right column

    /*
     * Value = value type
     * Owner = owner type
     * new has_many_to_many<Value, Owner>
     */
    // handle observer
    std::vector<Observer<has_one_to_many_item<Value, Owner> >*> has_many_item_observer;
    for (const auto &o : observer_vector_) {
      has_many_item_observer.push_back(new Observer<has_one_to_many_item<Value, Owner> >(o));
    }

    auto endpoint = std::make_shared<detail::has_one_to_many_endpoint <Value, Owner>>(id, &node_);

    node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

    // new has many to many item
    auto proto = new has_one_to_many_item<Value, Owner>(item_column, owner_column);
    prototype_node *node = prototype_node::make_relation_node<has_one_to_many_item<Value, Owner> >(&store_, id, proto, false, node_.type(), id);

    pi = store_.attach_internal<has_one_to_many_item<Value, Owner>>(node, nullptr, has_many_item_observer);
  } else {
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

      auto foreign_endpoint = std::make_shared<detail::right_to_many_endpoint <Owner, Value>>(id, foreign_node.get());

      foreign_node->register_relation_endpoint(std::type_index(typeid(Owner)), foreign_endpoint);

      std::vector<Observer<has_many_to_many_item<Owner, Value> >*> has_many_item_observer;
      for (auto o : observer_vector_) {
        has_many_item_observer.push_back(new Observer<has_many_to_many_item<Owner, Value> >(o));
      }

      auto endpoint = std::make_shared<detail::left_to_many_endpoint<Value, Owner>>(id, &node_);

      node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

      // link endpoints
      foreign_endpoint->foreign_endpoint = endpoint;
      endpoint->foreign_endpoint = foreign_endpoint;

      // new has many to many item
      auto proto = new has_many_to_many_item<Owner, Value>(item_column, owner_column);
      prototype_node *node = prototype_node::make_relation_node<has_many_to_many_item<Owner, Value>>(&store_, id, proto, false, node_.type(), id);

      pi = store_.attach_internal<has_many_to_many_item<Owner, Value>>(node, nullptr, has_many_item_observer);
    } else {

      // found corresponding belongs_to
      auto j = pi->find_endpoint(node_.type_index());
      if (j == pi->endpoint_end()) {
        // check for has many item
        throw_object_exception("prototype already inserted: " << pi->type());
      } else if (j->second->type == detail::basic_relation_endpoint::BELONGS_TO) {
        // replace foreign endpoint
        auto foreign_endpoint = std::make_shared<detail::belongs_to_many_endpoint <Owner, Value>>(j->second->field, pi.get());
        pi->unregister_relation_endpoint(node_.type_index());
        pi->register_relation_endpoint(node_.type_index(), foreign_endpoint);

        // create and register endpoint
//        std::cout << "serialize HAS_MANY $$ creating detail::many_to_one_endpoint<Value: " << typeid(Value).name() << ", Owner: " << typeid(Owner).name() << ">\n";
        auto endpoint = std::make_shared<detail::many_to_one_endpoint <Value, Owner>>(id, &node_);
        node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

        // link both endpoints
        foreign_endpoint->foreign_endpoint = endpoint;
        endpoint->foreign_endpoint = foreign_endpoint;
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
//  std::cout << "ANALYZING " << typeid(Owner).name() << " found HAS_MANY<" << typeid(Value).name() << "> (builtin)\n";
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

    auto proto = new has_one_to_many_item<Value, Owner>(item_column, owner_column);
    prototype_node *node = prototype_node::make_relation_node<has_one_to_many_item<Value, Owner> >(&store_, id, proto, false, node_.type(), id);

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
  store_.detach(node);
  // remove registered endpoint from foreign site
  prototype_iterator foreign_node = store_.find<Value>();
  if (foreign_node != store_.end()) {
    foreign_node->unregister_relation_endpoint(node_.type_index());
  }
  return foreign_node;
}

}
}