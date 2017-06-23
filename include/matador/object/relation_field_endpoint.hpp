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

  virtual void insert_value(object_proxy *value, object_proxy *owner) = 0;
  virtual void remove_value(object_proxy *value, object_proxy *owner) = 0;

  void insert_value_into_foreign(object_proxy *value, object_proxy *owner);
  template < class T >
  void insert_value_into_foreign(const object_ptr<T> &value, object_proxy *owner);
  void remove_value_from_foreign(object_proxy *value, object_proxy *owner);
  template < class T >
  void remove_value_from_foreign(object_proxy *value, const object_ptr<T> &owner);

  template < class T >
  void set_has_many_item_proxy(has_many_item_holder<T> &holder, const object_holder &obj);

  template < class T >
  void set_has_many_item_proxy(has_many_item_holder<T> &holder, object_proxy *proxy);

  std::string field;
  std::string type_name;
  prototype_node *node = nullptr;
  relation_type type;
  std::weak_ptr<basic_relation_endpoint> foreign_endpoint;

  // states
  bool is_inserting = false;
  bool is_removing = false;

};

template < class Value >
struct relation_endpoint : public basic_relation_endpoint
{
  relation_endpoint(const std::string &field, prototype_node *node, relation_type type)
    : basic_relation_endpoint(field, node, type)
  {}

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) = 0;
  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) = 0;
};

template < class Value, class Owner, template < class... > class HasManyItem >
struct to_many_endpoint : public relation_endpoint<Value>
{
  to_many_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::HAS_MANY)
  {}

  std::string owner_column;
  std::string item_column;

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
};

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

  virtual void insert_value(object_proxy *value, object_proxy *owner) override;

  virtual void remove_value(object_proxy */*value*/, object_proxy *owner) override;
};

template < class Value, class Owner >
using has_one_endpoint = from_one_endpoint<Value, Owner, basic_relation_endpoint::HAS_ONE >;

template < class Value, class Owner >
using belongs_to_one_endpoint = from_one_endpoint<Value, Owner, basic_relation_endpoint::BELONGS_TO >;

template < class Value, class Owner >
struct to_one_endpoint : public relation_endpoint<Value>
{
  to_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::HAS_MANY)
  {}

  relation_endpoint_value_inserter<Value> inserter;
  relation_endpoint_value_remover<Value> remover;

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;

  virtual void insert_value(object_proxy *value, object_proxy *owner) override;
  virtual void remove_value(object_proxy *value, object_proxy *owner) override;
};

template < class Value, class Owner >
struct belongs_to_many_endpoint : public relation_endpoint<Value>
{
  belongs_to_many_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::HAS_MANY)
  {}

  relation_endpoint_value_inserter<Owner> inserter;
  relation_endpoint_value_remover<Owner> remover;

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;

  virtual void insert_value(object_proxy *value, object_proxy *owner) override;

  virtual void remove_value(object_proxy */*value*/, object_proxy *owner) override;
};
/// @endcond

}
}

#endif //MATADOR_RELATION_FIELD_ENDPOINT_HPP
