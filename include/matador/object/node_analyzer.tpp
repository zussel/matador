#include "matador/object/node_analyzer.hpp"
#include "matador/object/prototype_iterator.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/container_item_holder.hpp"
#include "matador/object/to_many_endpoints.hpp"
#include "matador/object/observer_list_copy_creator.hpp"
#include "matador/object/relation_endpoint_validator.hpp"

namespace matador::detail {

template<class Owner, template <typename> typename... ObserverType>
template<class Value>
void node_analyzer<Owner, ObserverType...>::on_belongs_to(const char *id, object_ptr<Value> &x, const utils::foreign_attributes &/*attr*/)
{
  // find foreign_node of belongs to type
  prototype_iterator foreign_node = store_.find(x.type());
  if (foreign_node != store_.end()) {
    /*
     * foreign_node is already inserted
     * check if foreign_node was created from container
     * check if foreign_node container relation for id (id == table name)
     */
    auto i = foreign_node->find_endpoint(node_.type_index());
    if (i != foreign_node->endpoint_end()) {

      if (i->second->is_has_many()) {
        // yes, foreign_node was created from container!
        // detach current foreign_node (has_many_item == relation table)
        auto endpoint = std::make_shared<detail::belongs_to_many_endpoint<Value, Owner>>(id, &node_);
        node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

        endpoint->foreign_endpoint = i->second;
        i->second->foreign_endpoint = endpoint;
      } else if (i->second->is_has_one()) {
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

template<class Owner, template <typename> typename... ObserverType>
template<class Value>
void node_analyzer<Owner, ObserverType...>::on_has_one(const char *id, object_ptr<Value> &x, const utils::foreign_attributes &/*attr*/)
{
  auto endpoint = std::make_shared<detail::has_one_endpoint<Value, Owner>>(id, &node_);
  node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

  prototype_iterator foreign_node = store_.find(x.type());

  if (foreign_node != store_.end()) {
    auto i = foreign_node->find_endpoint(node_.type_index());
    if (i != foreign_node->endpoint_end()) {
      if (i->second->is_belongs_to()) {
        // link both nodes
        endpoint->foreign_endpoint = i->second;
        i->second->foreign_endpoint = endpoint;
      } else {
        throw object_exception("invalid endpoint type");
      }
    }
  }
}

template<class Owner, template <typename> typename... ObserverType>
template<class Value, template<class ...> class Container>
void node_analyzer<Owner, ObserverType...>::on_has_many(const char * /*id*/,
                                                        container<Value, Container> &/*x*/,
                                                        const char *join_column,
                                                        const utils::foreign_attributes &/*attr*/,
                                                        typename std::enable_if<!is_builtin<Value>::value>::type*)
{
  auto it = store_.find<Value>();
  if (it == store_.end()) {
    // validate that join column is available in foreign entity
    if (!relation_endpoint_validator<Value>::has_belongs_to(join_column)) {
      throw_object_exception("foreign object hasn't join column '" << join_column << "' for relation type BELONGS_TO");
    }

    auto endpoint = std::make_shared<detail::many_to_one_endpoint<Value, Owner>>(join_column, &node_);
//    endpoint->foreign_endpoint = foreign_endpoint;
    node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

    // handle observer
//    using has_many_item = has_one_to_many_item_foreign<Owner, Value>;
//
//    auto endpoint = std::make_shared<detail::has_one_to_many_endpoint<Owner, typename has_many_item::right_value_type>>(id, &node_);
//    node_.register_relation_endpoint(std::type_index(typeid(typename has_many_item::right_value_type)), endpoint);

    // new has-one-to-many item
//    auto owner_column_name = node_.type() + std::string("_id");
//    auto proto = new has_many_item(owner_column_name, join_column);

//    auto node = prototype_node::make_relation_node<has_many_item>(store_, id, proto, prototype_node::abstract_type::not_abstract, node_.type(), id);

//    auto observers = observer_list_copy_creator<Owner, has_many_item, ObserverType...>::copy_create(observers_);
//    it = store_.attach_internal<has_many_item, ObserverType...>(node.release(), nullptr, std::move(observers));
//    if (it == store_.end()) {
//      throw_object_exception("couldn't insert prototype node: " << id);
//    }

//    auto sep = it->find_endpoint(id);
//    if (sep != it->endpoint_end()) {
//      sep->second->foreign_endpoint = endpoint;
//    }
  } else {
    auto ep = it->endpoints();
    // found corresponding belongs_to
    auto j = it->find_endpoint(node_.type_index());
    if (j == it->endpoint_end()) {
      // check for has many item
      throw_object_exception("prototype already inserted: " << it->type());
    } else if (j->second->is_belongs_to()) {
      // replace foreign endpoint
      auto foreign_endpoint = std::make_shared<detail::belongs_to_many_endpoint<Owner, Value>>(j->second->field, it.get());
      it->unregister_relation_endpoint(node_.type_index());
      it->register_relation_endpoint(node_.type_index(), foreign_endpoint);

      // create and register endpoint
      // here
      auto endpoint = std::make_shared<detail::many_to_one_endpoint<Value, Owner>>(join_column, &node_);
      endpoint->foreign_endpoint = foreign_endpoint;
      node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

      // link both endpoints
      foreign_endpoint->foreign_endpoint = endpoint;
    }
  }
}

template<class Owner, template <typename> typename... ObserverType>
template<class Value, template<class ...> class Container>
void node_analyzer<Owner, ObserverType...>::on_has_many(const char *id,
                                                        container<Value, Container> &x,
                                                        const char *join_column,
                                                        const utils::foreign_attributes &/*attr*/,
                                                        typename std::enable_if<is_builtin<Value>::value>::type*)
{
  // attach relation table for has many relation
  // check if it has many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    using has_many_item = has_one_to_many_item_scalar<Owner, Value>;

    auto endpoint = std::make_shared<detail::has_one_to_many_endpoint<Owner, typename has_many_item::right_value_type>>(id, &node_);
    node_.register_relation_endpoint(std::type_index(typeid(typename has_many_item::right_value_type)), endpoint);

    auto proto = new has_many_item(node_.type() + std::string("_id"), join_column, x.size_);
    auto node = prototype_node::make_relation_node<has_many_item>(store_, id, proto, prototype_node::abstract_type::not_abstract, node_.type(), id);

    auto observers = observer_list_copy_creator<Owner, has_many_item, ObserverType...>::copy_create(observers_);
    pi = store_.attach_internal<has_many_item, ObserverType...>(node.release(), nullptr, std::move(observers));

    auto sep = pi->find_endpoint(join_column);
    if (sep != pi->endpoint_end()) {
      sep->second->foreign_endpoint = endpoint;
      endpoint->foreign_endpoint = sep->second;
    }
  } else {
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

template<class Owner, template <typename> typename... ObserverType>
template<class Value, template<class ...> class Container>
void node_analyzer<Owner, ObserverType...>::on_has_many(const char * id,
                                                        container<Value, Container> &/*x*/,
                                                        const utils::foreign_attributes &/*attr*/,
                                                        typename std::enable_if<!is_builtin<Value>::value>::type*)
{
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    using has_many_item = has_one_to_many_item_foreign<Owner, Value>;

    auto endpoint = std::make_shared<detail::has_one_to_many_endpoint<Owner, typename has_many_item::right_value_type>>(id, &node_);
    node_.register_relation_endpoint(std::type_index(typeid(typename has_many_item::right_value_type)), endpoint);

    std::string value_column = "value_id";
    auto proto = new has_many_item(node_.type(), value_column);
    auto node = prototype_node::make_relation_node<has_many_item>(store_, id, proto, prototype_node::abstract_type::not_abstract, node_.type(), id);

    auto observers = observer_list_copy_creator<Owner, has_many_item, ObserverType...>::copy_create(observers_);
    pi = store_.attach_internal<has_many_item, ObserverType...>(node.release(), nullptr, std::move(observers));

    auto sep = pi->find_endpoint(node_.type());
    if (sep != pi->endpoint_end()) {
      sep->second->foreign_endpoint = endpoint;
//      endpoint->foreign_endpoint = sep->second;
    }
  } else {
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

template<class Owner, template <typename> typename... ObserverType>
template<class Value, template<class ...> class Container>
void node_analyzer<Owner, ObserverType...>::on_has_many(const char * id,
                                                        container<Value, Container> &/*x*/,
                                                        const utils::foreign_attributes &/*attr*/,
                                                        typename std::enable_if<is_builtin<Value>::value>::type*)
{
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    using has_many_item = has_one_to_many_item_scalar<Owner, Value>;

    auto endpoint = std::make_shared<detail::has_one_to_many_endpoint<Owner, typename has_many_item::right_value_type>>(id, &node_);
    node_.register_relation_endpoint(std::type_index(typeid(typename has_many_item::right_value_type)), endpoint);

    std::string value_column = "value_id";
    auto proto = new has_many_item(node_.type(), value_column, utils::null_attributes);
    auto node = prototype_node::make_relation_node<has_many_item>(store_, id, proto, prototype_node::abstract_type::not_abstract, node_.type(), id);

    auto observers = observer_list_copy_creator<Owner, has_many_item, ObserverType...>::copy_create(observers_);
    pi = store_.attach_internal<has_many_item, ObserverType...>(node.release(), nullptr, std::move(observers));

    auto sep = pi->find_endpoint(node_.type());
    if (sep != pi->endpoint_end()) {
      sep->second->foreign_endpoint = endpoint;
      endpoint->foreign_endpoint = sep->second;
    }
  } else {
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

template<class Owner, template <typename> typename... ObserverType>
template<class Value, template<class ...> class Container>
void node_analyzer<Owner, ObserverType...>::on_has_many_to_many(const char *id,
                                                                container<Value, Container> &/*x*/,
                                                                const utils::foreign_attributes &/*attr*/)
{
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    using has_many_item = has_many_to_many_item<Value, Owner>;

    auto endpoint = std::make_shared<detail::right_to_many_endpoint<Value, Owner>>(id, &node_);
    node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

    // new has many to many item
    auto proto = new has_many_item;
    auto node = prototype_node::make_relation_node<has_many_item>(store_, id, proto, prototype_node::abstract_type::not_abstract, node_.type(), id);

    auto observers = observer_list_copy_creator<Owner, has_many_item, ObserverType...>::copy_create(observers_);
    pi = store_.attach_internal<has_many_item, ObserverType...>(node.release(), nullptr, std::move(observers));
    if (pi == store_.end()) {
      throw_object_exception("couldn't insert prototype node: " << id);
    }

    auto sep = pi->find_endpoint(node_.type_index());
    if (sep != pi->endpoint_end()) {
      sep->second->foreign_endpoint = endpoint;
    }
  } else if (pi->type_index() == typeid(has_many_to_many_item<Value, Owner>)) {
    // Value: student
    // Owner: course
    auto *proto = pi->prototype<has_many_to_many_item<Value, Owner>>();
    auto ep = pi->endpoints();

    auto endpoint = std::make_shared<detail::right_to_many_endpoint<Value, Owner>>(id, &node_);
    node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

    auto sep = pi->find_endpoint(proto->right_column());
    if (sep != pi->endpoint_end()) {
      sep->second->foreign_endpoint = endpoint;
    }

    sep = pi->find_endpoint(proto->left_column());
    if (sep != pi->endpoint_end()) {
      auto sptr = sep->second->foreign_endpoint.lock();
      if (sptr) {
        endpoint->foreign_endpoint = sptr;
        sptr->foreign_endpoint = endpoint;
      }
    }
  } else {
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

template<class Owner, template <typename> typename... ObserverType>
template<class Value, template<class ...> class Container>
void node_analyzer<Owner, ObserverType...>::on_has_many_to_many(const char *id, container<Value, Container> &,
                                                                const char *join_column,
                                                                const char *inverse_join_column,
                                                                const utils::foreign_attributes &/*attr*/)
                                       {
  // attach relation table for has many relation
  // check if has-many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = store_.find(id);
  if (pi == store_.end()) {
    // Value = foreign/owner type => join column
    // Owner = item/value type    => inverse join column

    /*
     * Value = value type
     * Owner = owner type
     * new has_many_to_many<Value, Owner>
     */
    using has_many_item = has_many_to_many_item<Owner, Value>;

    auto endpoint = std::make_shared<detail::left_to_many_endpoint<Value, Owner>>(id, &node_);
    node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

    // new has many to many item
    auto proto = new has_many_item(join_column, inverse_join_column);
    auto node = prototype_node::make_relation_node<has_many_item>(store_, id, proto, prototype_node::abstract_type::not_abstract, node_.type(), id);

    auto observers = observer_list_copy_creator<Owner, has_many_item, ObserverType...>::copy_create(observers_);
    pi = store_.attach_internal<has_many_item, ObserverType...>(node.release(), nullptr, std::move(observers));
    if (pi == store_.end()) {
      throw_object_exception("couldn't insert prototype node: " << id);
    }

    auto sep = pi->find_endpoint(join_column);
    if (sep != pi->endpoint_end()) {
      sep->second->foreign_endpoint = endpoint;
    }
  } else if (pi->type_index() == typeid(has_many_to_many_item<Owner, Value>)) {
    // found node
    auto endpoint = std::make_shared<detail::left_to_many_endpoint<Value, Owner>>(id, &node_);
    node_.register_relation_endpoint(std::type_index(typeid(Value)), endpoint);

    // update prototype columns
    auto *proto = pi->prototype<has_many_to_many_item<Owner, Value>>();
    auto sep = pi->find_endpoint(node_.type_index());
    if (sep != pi->endpoint_end()) {
      sep->second->foreign_endpoint = endpoint;
      sep->second->field = join_column;
      proto->left_column(join_column);
    } else {
      throw_object_exception("couldn't find endpoint for: " << join_column);
    }

    sep = pi->find_endpoint(typeid(Value));
    if (sep == pi->endpoint_end()) {
      throw_object_exception("couldn't find endpoint for: " << inverse_join_column);
    }
    sep->second->field = inverse_join_column;
    proto->right_column(inverse_join_column);
    auto foreign_endpoint = sep->second->foreign_endpoint.lock();
    if (!foreign_endpoint) {
      throw_object_exception("couldn't find endpoint for: " << inverse_join_column);
    }

    // link endpoints
    foreign_endpoint->foreign_endpoint = endpoint;
    endpoint->foreign_endpoint = foreign_endpoint;

  } else {
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

}
