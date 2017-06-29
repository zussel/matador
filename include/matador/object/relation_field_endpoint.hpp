//
// @author sascha on 3/23/17.
//

#ifndef MATADOR_RELATION_FIELD_ENDPOINT_HPP
#define MATADOR_RELATION_FIELD_ENDPOINT_HPP

#ifdef _MSC_VER
#ifdef matador_object_EXPORTS
#define MATADOR_OBJECT_API __declspec(dllexport)
#define EXPIMP_OBJECT_TEMPLATE
#else
#define MATADOR_OBJECT_API __declspec(dllimport)
#define EXPIMP_OBJECT_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#else
#define MATADOR_OBJECT_API
#endif

#include "matador/object/relation_field_serializer.hpp"
#include "matador/object/relation_endpoint_value_inserter.hpp"
#include "matador/object/relation_endpoint_value_remover.hpp"
#include "matador/object/basic_has_many_to_many_item.hpp"
#include "matador/object/has_many_item_holder.hpp"

#include <string>
#include <functional>
#include <memory>

namespace matador {

class object_proxy;
class prototype_node;
class object_store;

namespace detail {

/// @cond MATADOR_DEV

/**
 * Basic relation endpoint providing the interface
 */
struct basic_relation_endpoint
{
  enum relation_type {
    BELONGS_TO, HAS_ONE, HAS_MANY
  };

  basic_relation_endpoint(const std::string &fld, prototype_node *n, relation_type t)
    : field(fld), node(n), type(t)
  {
    switch (type) {
      case BELONGS_TO: type_name = "belongs_to"; break;
      case HAS_ONE: type_name = "has_one"; break;
      case HAS_MANY: type_name = "has_many"; break;
      default: break;
    }
  }
  virtual ~basic_relation_endpoint() {}

  void insert_value(object_proxy *value, object_proxy *owner);
  void remove_value(object_proxy *value, object_proxy *owner);
  virtual void insert_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) = 0;
  virtual void remove_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) = 0;

  void insert_value_into_foreign(object_proxy *value, object_proxy *owner, object_proxy *item_proxy);
  template < class Value >
  void insert_value_into_foreign(const has_many_item_holder<Value> &holder, object_proxy *owner);
  void remove_value_from_foreign(object_proxy *value, object_proxy *owner, object_proxy *item_proxy);
  template < class Value >
  void remove_value_from_foreign(const has_many_item_holder<Value> &value, object_proxy *owner);

  template < class T >
  void set_has_many_item_proxy(has_many_item_holder<T> &holder, const object_holder &obj);

  template < class T >
  void set_has_many_item_proxy(has_many_item_holder<T> &holder, object_proxy *proxy);

  std::string field;
  std::string type_name;
  prototype_node *node = nullptr;
  relation_type type;
  std::weak_ptr<basic_relation_endpoint> foreign_endpoint;
};

/**
 * relation endpoint interface with value type
 * @tparam Value
 */
template < class Value >
struct relation_endpoint : public basic_relation_endpoint
{
  relation_endpoint(const std::string &field, prototype_node *node, relation_type type)
    : basic_relation_endpoint(field, node, type)
  {}

  virtual void insert_value(const has_many_item_holder<Value> &value, object_proxy *owner) = 0;
  virtual void remove_value(const has_many_item_holder<Value> &value, object_proxy *owner) = 0;

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) = 0;
  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) = 0;
};

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

/**
 * Base class for from one relation endpoints
 * like 'has_one' and 'belongs_to_one' endpoints
 *
 * @tparam Value
 * @tparam Owner
 * @tparam Type
 */
template < class Value, class Owner, basic_relation_endpoint::relation_type Type>
struct from_one_endpoint : public relation_endpoint<Value>
{
  from_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, Type)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;

  virtual void insert_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override;
  virtual void remove_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override;
  virtual void insert_value(const has_many_item_holder<Value> &value, object_proxy *owner) override;
  virtual void remove_value(const has_many_item_holder<Value> &value, object_proxy *owner) override;
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
  many_to_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::HAS_MANY)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;

  virtual void insert_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override;
  virtual void remove_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override;
  virtual void insert_value(const has_many_item_holder<Value> &value, object_proxy *owner) override;
  virtual void remove_value(const has_many_item_holder<Value> &value, object_proxy *owner) override;
};

template < class Value, class Owner >
struct many_to_one_endpoint<Value, Owner, typename std::enable_if<std::is_base_of<basic_has_many_to_many_item, Value>::value>::type>
  : public relation_endpoint<Value>
{
  many_to_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::HAS_MANY)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  virtual void insert_holder(object_store &/*store*/, has_many_item_holder<Value> &/*holder*/, object_proxy */*owner*/) override
  {
    std::cout << "endpoint for field " << this->field << ": called INSERT_HOLDER (Value: " << typeid(Value).name() << ", Owner:" << typeid(Owner).name() << ")\n";
  }
  virtual void remove_holder(object_store &/*store*/, has_many_item_holder<Value> &/*holder*/, object_proxy */*owner*/) override
  {
    std::cout << "endpoint for field " << this->field << ": called REMOVE_HOLDER (Value: " << typeid(Value).name() << ", Owner:" << typeid(Owner).name() << ")\n";
  }

  virtual void insert_value(object_proxy */*value*/, object_proxy */*owner*/, object_proxy */*item_proxy*/) override
  {
    std::cout << "endpoint for field " << this->field << ": called INSERT_VALUE (Value: " << typeid(Value).name() << ", Owner:" << typeid(Owner).name() << ")\n";
  }
  virtual void remove_value(object_proxy */*value*/, object_proxy */*owner*/, object_proxy */*item_proxy*/) override
  {
    std::cout << "endpoint for field " << this->field << ": called REMOVE_VALUE (Value: " << typeid(Value).name() << ", Owner:" << typeid(Owner).name() << ")\n";
  }
};

template < class Value, class Owner >
struct belongs_to_many_endpoint : public relation_endpoint<Value>
{
  belongs_to_many_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::BELONGS_TO)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;

  virtual void insert_value(object_proxy *value, object_proxy *owner, object_proxy *item_proxy) override;
  virtual void remove_value(object_proxy */*value*/, object_proxy *owner, object_proxy *item_proxy) override;
};
/// @endcond

}
}

#endif //MATADOR_RELATION_FIELD_ENDPOINT_HPP
