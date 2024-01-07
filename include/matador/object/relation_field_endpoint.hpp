#ifndef MATADOR_RELATION_FIELD_ENDPOINT_HPP
#define MATADOR_RELATION_FIELD_ENDPOINT_HPP

#include "matador/object/export.hpp"

#include "matador/utils/is_builtin.hpp"

#include "matador/object/relation_endpoint_value_inserter.hpp"
#include "matador/object/relation_endpoint_value_remover.hpp"
#include "matador/object/relation_endpoint.hpp"
#include "matador/object/basic_has_many_to_many_item.hpp"

#include <string>
#include <functional>
#include <memory>
#include <iosfwd>

namespace matador {

class object_proxy;
class prototype_node;
class object_store;

namespace detail {

/// @cond MATADOR_DEV

/**
 * Base class for to_many relation endpoints
 * providing value and owner type
 *
 * @tparam Value
 * @tparam Owner
 */
template < class Value, class Owner >
struct from_many_endpoint : public relation_endpoint<Value>
{
  from_many_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::HAS_MANY)
  {}

  std::string owner_column;
  std::string item_column;
};

template < class Value, class Owner, basic_relation_endpoint::relation_type Type, class Enabled = void >
struct from_one_endpoint;

/**
 * Base class for from one relation endpoints
 * like 'has_one' and 'belongs_to_one' endpoints
 *
 * @tparam Value
 * @tparam Owner
 * @tparam Type
 */
template < class Value, class Owner, basic_relation_endpoint::relation_type Type>
struct from_one_endpoint<Value, Owner, Type, typename std::enable_if<!matador::is_builtin<Value>::value>::type>
  : public relation_endpoint<Value>
{
  from_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, Type)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override;
  void remove_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override;

  void insert_value(object_proxy *value, object_proxy *owner) override;
  void remove_value(object_proxy *value, object_proxy *owner) override;

  void insert_value(const basic_container_item_holder &holder, object_proxy *owner) override;
  void remove_value(const basic_container_item_holder &holder, object_proxy *owner) override;

  object_proxy* acquire_proxy(unsigned long long , object_store &) override { return nullptr; }
};

template < class Value, class Owner, basic_relation_endpoint::relation_type Type>
struct from_one_endpoint<Value, Owner, Type, typename std::enable_if<matador::is_builtin<Value>::value>::type>
  : public relation_endpoint<Value>
{
  from_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, Type)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override;
  void remove_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override;

  void insert_value(object_proxy *value, object_proxy *owner) override;
  void remove_value(object_proxy *value, object_proxy *owner) override;

  void insert_value(const basic_container_item_holder &holder, object_proxy *owner) override;
  void remove_value(const basic_container_item_holder &holder, object_proxy *owner) override;

  object_proxy* acquire_proxy(unsigned long long , object_store &) override { return nullptr; }
};

template < class Value, class Owner >
using has_one_endpoint = from_one_endpoint<Value, Owner, basic_relation_endpoint::HAS_ONE >;

template < class Value, class Owner >
using belongs_to_one_endpoint = from_one_endpoint<Value, Owner, basic_relation_endpoint::BELONGS_TO >;

template < class Value, class Owner, class Enabled = void >
struct many_to_one_endpoint;

template < class Value, class Owner >
struct many_to_one_endpoint<Value, Owner, typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, Value>::value>::type>
  : public relation_endpoint<Value>
{
  typedef container_item_holder<Value> value_type;

  many_to_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::HAS_MANY)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override;
  void remove_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override;

  void insert_value(const basic_container_item_holder &holder, object_proxy *owner) override;
  void remove_value(const basic_container_item_holder &holder, object_proxy *owner) override;

  void insert_value(object_proxy *value, object_proxy *owner) override;
  void remove_value(object_proxy *value, object_proxy *owner) override;

  object_proxy* acquire_proxy(unsigned long long , object_store &) override { return nullptr; }

};

template < class Value, class Owner >
struct many_to_one_endpoint<Value, Owner, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, Value>::value>::type>
  : public relation_endpoint<Value>
{
  many_to_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::HAS_MANY)
  { }

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &, container_item_holder<Value> &, object_proxy *) override {} // owner
  void remove_holder(object_store &, container_item_holder<Value> &, object_proxy *) override {} // owner

  void insert_value(object_proxy *, object_proxy *) override {} // value, owner
  void remove_value(object_proxy *, object_proxy *) override {} // value, owner

  void insert_value(const basic_container_item_holder &, object_proxy *) override {} // owner
  void remove_value(const basic_container_item_holder &, object_proxy *) override {} // owner

  object_proxy* acquire_proxy(unsigned long long , object_store &) override { return nullptr; }
};

template < class Value, class Owner, class Enabled = void >
struct belongs_to_many_endpoint;

template < class Value, class Owner >
struct belongs_to_many_endpoint<Value, Owner, typename std::enable_if<!matador::is_builtin<Value>::value>::type>
  : public relation_endpoint<Value>
{
  belongs_to_many_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::BELONGS_TO)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override;
  void remove_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) override;

  void insert_value(object_proxy *value, object_proxy *owner) override;
  void remove_value(object_proxy *value, object_proxy *owner) override;

  void insert_value(const basic_container_item_holder &holder, object_proxy *owner) override;
  void remove_value(const basic_container_item_holder &holder, object_proxy *owner) override;

  object_proxy* acquire_proxy(unsigned long long , object_store &) override { return nullptr; }

  void print(std::ostream &out) const override;
};

template < class Value, class Owner >
struct belongs_to_many_endpoint<Value, Owner, typename std::enable_if<matador::is_builtin<Value>::value>::type>
  : public relation_endpoint<Value>
{
  belongs_to_many_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::BELONGS_TO)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  void insert_holder(object_store &, container_item_holder<Value> &, object_proxy *) override {} // owner
  void remove_holder(object_store &, container_item_holder<Value> &, object_proxy *) override {} // owner

  void insert_value(object_proxy *, object_proxy *) override {} // value, owner
  void remove_value(object_proxy *, object_proxy *) override {} // value, owner

  void insert_value(const basic_container_item_holder &, object_proxy *) override {} // owner
  void remove_value(const basic_container_item_holder &, object_proxy *) override {} // owner

  object_proxy* acquire_proxy(unsigned long long , object_store &) override { return nullptr; }
};

template < class Value, class Owner, basic_relation_endpoint::relation_type Type >
void from_one_endpoint<
Value, Owner, Type,typename std::enable_if<!matador::is_builtin<Value>::value>::type
>::insert_holder(object_store &, container_item_holder<Value> &holder, object_proxy *owner)
{
  this->set_has_many_item_proxy(holder, owner);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type >
void from_one_endpoint<
Value, Owner, Type, typename std::enable_if<!matador::is_builtin<Value>::value>::type
>::remove_holder(object_store &, container_item_holder<Value> &holder, object_proxy *) // owner
{
  this->set_has_many_item_proxy(holder, nullptr);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type >
void from_one_endpoint<
Value, Owner, Type, typename std::enable_if<!matador::is_builtin<Value>::value>::type
>::insert_value(object_proxy *value, object_proxy *owner)
{
  insert_value(container_item_holder<Value>(value, nullptr), owner);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type >
void from_one_endpoint<
Value, Owner, Type, typename std::enable_if<!matador::is_builtin<Value>::value>::type
>::remove_value(object_proxy *value, object_proxy *owner)
{
  remove_value(container_item_holder<Value>(value, nullptr), owner);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type>
void from_one_endpoint<
Value, Owner, Type, typename std::enable_if<!matador::is_builtin<Value>::value>::type
>::insert_value(const basic_container_item_holder &holder, object_proxy *owner)
{
  object_ptr<Owner> ownptr(owner);
  const auto &value_holder = static_cast<const container_item_holder<Value>&>(holder);
  inserter.insert(ownptr, this->field, value_holder);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type>
void from_one_endpoint<
Value, Owner, Type, typename std::enable_if<!matador::is_builtin<Value>::value>::type
>::remove_value(const basic_container_item_holder &holder, object_proxy *owner)
{
  object_ptr<Owner> ownptr(owner);
  const auto &value_holder = static_cast<const container_item_holder<Value>&>(holder);
  remover.remove(ownptr, this->field, value_holder);
}


template < class Value, class Owner, basic_relation_endpoint::relation_type Type >
void from_one_endpoint<
Value, Owner, Type,typename std::enable_if<matador::is_builtin<Value>::value>::type
>::insert_holder(object_store &, container_item_holder<Value> &holder, object_proxy *owner)
{
  this->set_has_many_item_proxy(holder, owner);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type >
void from_one_endpoint<
Value, Owner, Type, typename std::enable_if<matador::is_builtin<Value>::value>::type
>::remove_holder(object_store &, container_item_holder<Value> &holder, object_proxy *) // owner
{
  this->set_has_many_item_proxy(holder, nullptr);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type >
void from_one_endpoint<
Value, Owner, Type, typename std::enable_if<matador::is_builtin<Value>::value>::type
>::insert_value(object_proxy *value, object_proxy *owner)
{
  insert_value(container_item_holder<Value>(value, nullptr), owner);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type >
void from_one_endpoint<
Value, Owner, Type, typename std::enable_if<matador::is_builtin<Value>::value>::type
>::remove_value(object_proxy *value, object_proxy *owner)
{
  remove_value(container_item_holder<Value>(value, nullptr), owner);
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type>
void from_one_endpoint<
Value, Owner, Type, typename std::enable_if<matador::is_builtin<Value>::value>::type
>::insert_value(const basic_container_item_holder &holder, object_proxy *owner)
{
  object_ptr<Owner> ownptr(owner);
  inserter.insert(ownptr, this->field, static_cast<const container_item_holder<Value>&>(holder));
}

template < class Value, class Owner, basic_relation_endpoint::relation_type Type>
void from_one_endpoint<
Value, Owner, Type, typename std::enable_if<matador::is_builtin<Value>::value>::type
>::remove_value(const basic_container_item_holder &holder, object_proxy *owner)
{
  object_ptr<Owner> ownptr(owner);
  remover.remove(ownptr, this->field, static_cast<const container_item_holder<Value>&>(holder));
}

template < class Value, class Owner >
void belongs_to_many_endpoint<
Value, Owner, typename std::enable_if<!matador::is_builtin<Value>::value>::type
>::insert_holder(object_store &, container_item_holder<Value> &holder, object_proxy *owner)
{
  this->set_has_many_item_proxy(holder, owner);
}

template < class Value, class Owner >
void belongs_to_many_endpoint<
Value, Owner, typename std::enable_if<!matador::is_builtin<Value>::value>::type
>::remove_holder(object_store &, container_item_holder<Value> &holder, object_proxy *) // owner
{
  this->set_has_many_item_proxy(holder, nullptr);
}

template < class Value, class Owner >
void belongs_to_many_endpoint<Value, Owner, typename std::enable_if<!matador::is_builtin<Value>::value>::type>::insert_value(object_proxy *value, object_proxy *owner)
{
  object_ptr<Owner> valptr(value);
  inserter.insert(valptr, this->field, container_item_holder<Value>(owner, nullptr));
  this->proxy(valptr)->mark_modified();
}

template < class Value, class Owner >
void belongs_to_many_endpoint<Value, Owner, typename std::enable_if<!matador::is_builtin<Value>::value>::type>::remove_value(object_proxy *value, object_proxy *owner)
{
  object_ptr<Owner> valptr(value);
  remover.remove(valptr, this->field, container_item_holder<Value>(owner, nullptr));
  this->proxy(valptr)->mark_modified();
}

template < class Value, class Owner >
void belongs_to_many_endpoint<Value, Owner, typename std::enable_if<!matador::is_builtin<Value>::value>::type>::insert_value(const basic_container_item_holder &holder, object_proxy *owner)
{
  inserter.insert(static_cast<const container_item_holder<Owner>&>(holder).value(),
                  this->field,
                  container_item_holder<Value>(owner, nullptr));
  this->proxy(static_cast<const container_item_holder<Owner>&>(holder).value())->mark_modified();
}

template < class Value, class Owner >
void belongs_to_many_endpoint<Value, Owner, typename std::enable_if<!matador::is_builtin<Value>::value>::type>::remove_value(const basic_container_item_holder &holder, object_proxy *owner)
{
  remover.remove(static_cast<const container_item_holder<Owner> &>(holder).value(),
                 this->field,
                 container_item_holder<Value>(owner, nullptr));
  this->proxy(static_cast<const container_item_holder<Owner> &>(holder).value())->mark_modified();
}

template<class Value, class Owner>
void belongs_to_many_endpoint<Value, Owner, typename std::enable_if<!matador::is_builtin<Value>::value>::type>::print(std::ostream &out) const
{
  out << "relation belongs_to_many_endpoint<" << typeid(Value).name() << "," << typeid(Owner).name() << "> " << this->node->type() << "::" << this->field << " (" << this->type_name << ")";
  auto locked_endpoint = this->foreign_endpoint.lock();
  if (locked_endpoint) {
    out << " -> " << locked_endpoint->node->type() << "::" << locked_endpoint->field << " (" << locked_endpoint->type_name << ")";
  }
}

template < class Value, class Owner >
void many_to_one_endpoint<
Value, Owner, typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, Value>::value>::type
>::insert_holder(object_store &, container_item_holder<Value> &holder, object_proxy *owner)
{
  this->mark_holder_as_inserted(holder);
  this->set_has_many_item_proxy(holder, owner);
  this->increment_reference_count(holder.value());
}

template < class Value, class Owner >
void many_to_one_endpoint<Value, Owner,
typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, Value>::value>::type
>::remove_holder(object_store &, container_item_holder<Value> &holder, object_proxy *) // owner
{
  this->set_has_many_item_proxy(holder, nullptr);
  this->decrement_reference_count(holder.value());
}

template < class Value, class Owner >
void many_to_one_endpoint<Value, Owner,
typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, Value>::value>::type
>::insert_value(object_proxy *value, object_proxy *owner)
{
  insert_value(value_type(value, nullptr), owner);
}

template < class Value, class Owner >
void many_to_one_endpoint<Value, Owner,
typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, Value>::value>::type
>::remove_value(object_proxy *value, object_proxy *owner)
{
  remove_value(value_type(value, nullptr), owner);
}

template < class Value, class Owner >
void many_to_one_endpoint<Value, Owner,
typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, Value>::value>::type
>::insert_value(const basic_container_item_holder &holder, object_proxy *owner)
{
  const auto &value_holder = static_cast<const container_item_holder<Value>&>(holder);

  object_ptr<Owner> ownptr(owner);
  this->mark_holder_as_inserted(const_cast<container_item_holder<Value>&>(value_holder));
  inserter.insert(ownptr, this->field, value_holder);
//  this->increment_reference_count(value_holder.value());
}

template < class Value, class Owner >
void many_to_one_endpoint<Value, Owner,
typename std::enable_if<!std::is_base_of<basic_has_many_to_many_item, Value>::value>::type
>::remove_value(const basic_container_item_holder &holder, object_proxy *owner)
{
  const auto &value_holder = static_cast<const container_item_holder<Value>&>(holder);

  object_ptr<Owner> ownptr(owner);
  remover.remove(ownptr, this->field, value_holder);
//  this->decrement_reference_count(value_holder.value());
}
/// @endcond

}
}

#endif //MATADOR_RELATION_FIELD_ENDPOINT_HPP
