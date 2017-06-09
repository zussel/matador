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
#include "matador/object/relation_endpoint_serializer.hpp"
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
//struct MATADOR_OBJECT_API relation_field_endpoint
//{
//  enum relation_type {
//    BELONGS_TO, HAS_ONE, HAS_MANY
//  };
//
//  enum has_many_relation_side {
//    LEFT, RIGHT
//  };
//  relation_field_endpoint(const std::string &f, relation_type t, prototype_node *pn);
//  ~relation_field_endpoint();
//
//  template < class T >
//  void set(object_store &store, const object_ptr<T> &owner, object_proxy *value);
//  template < class T >
//  void clear(object_store &store, const object_ptr<T> &owner);
//  template < class T >
//  void append(object_store &store, const object_ptr<T> &owner, object_proxy *value);
//  template < class T >
//  void remove(object_store &store, const object_ptr<T> &owner, object_proxy *value);
//
//  template < class T >
//  void create(object_store &store, const object_ptr<T> &value, object_proxy *owner);
//
//  template < class T, class V >
//  void insert(object_store &store, const object_ptr<T> &value, object_proxy *owner);
//
//  template < class Value, class Owner >
//  void insert(object_store &store, has_many_item_holder<Value> &holder);
//
//  std::string field;
//  relation_type type;
//  has_many_relation_side side = LEFT;
//  prototype_node *node = nullptr;
//  std::weak_ptr<relation_field_endpoint> foreign_endpoint;
//  bool is_insert_in_progress = false;
//  bool is_remove_in_progress = false;
//
//  relation_field_serializer field_serializer;
//};

struct basic_relation_endpoint
{
  enum relation_type {
    BELONGS_TO, HAS_ONE, HAS_MANY
  };

  basic_relation_endpoint(const std::string &fld, prototype_node *n, relation_type t)
    : field(fld), node(n), type(t)
  {}
  virtual ~basic_relation_endpoint() {}

  virtual void insert_value(object_proxy *value, object_proxy *owner) = 0;
  virtual void remove_value(object_proxy *value, object_proxy *owner) = 0;

  template < class T >
  void set_has_many_item_proxy(has_many_item_holder<T> &holder, const object_holder &obj);

  template < class T >
  void set_has_many_item_proxy(has_many_item_holder<T> &holder, object_proxy *proxy);

  std::string field;
  prototype_node *node = nullptr;
  relation_type type;
  std::weak_ptr<basic_relation_endpoint> foreign_endpoint;
};

template < class Value >
struct relation_endpoint : public basic_relation_endpoint
{
  relation_endpoint(const std::string &field, prototype_node *node, relation_type type)
    : basic_relation_endpoint(field, node, type)
  {}

  relation_endpoint_serializer<Value> modifier;

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

  virtual void insert_value(object_proxy *value, object_proxy *owner) override
  {
    object_ptr<Value> valptr(value);
    object_ptr<Owner> ownptr(owner);
    this->modifier.set(ownptr, "", valptr);
  }

  virtual void remove_value(object_proxy *value, object_proxy *owner) override
  {
    object_ptr<Value> valptr(value);
    object_ptr<Owner> ownptr(owner);
    this->modifier.set(ownptr, "", valptr);
  }
};

template < class Value, class Owner >
struct has_one_endpoint : public relation_endpoint<Value>
{
  has_one_endpoint(const std::string &field, prototype_node *node, basic_relation_endpoint::relation_type type)
    : relation_endpoint<Value>(field, node, type)
  {}

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;

  virtual void insert_value(object_proxy *value, object_proxy *owner) override
  {
    object_ptr<Value> valptr(value);
    object_ptr<Owner> ownptr(owner);
    this->modifier.set(ownptr, "", valptr);
  }

  virtual void remove_value(object_proxy *value, object_proxy */*owner*/) override
  {
    object_ptr<Value> valptr;
    object_ptr<Owner> ownptr(value);
    this->modifier.set(ownptr, "", valptr);
  }
};

template < class Value, class Owner >
struct to_one_endpoint : public relation_endpoint<Value>
{
  to_one_endpoint(const std::string &field, prototype_node *node)
    : relation_endpoint<Value>(field, node, basic_relation_endpoint::HAS_MANY)
  {}

  virtual void insert_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;
  virtual void remove_holder(object_store &store, has_many_item_holder<Value> &holder, object_proxy *owner) override;

  virtual void insert_value(object_proxy *value, object_proxy *owner) override
  {
    object_ptr<Value> valptr(value);
    object_ptr<Owner> ownptr(owner);
    this->modifier.set(ownptr, "", valptr);
  }

  virtual void remove_value(object_proxy *value, object_proxy */*owner*/) override
  {
    object_ptr<Value> valptr;
    object_ptr<Owner> ownptr(value);
    this->modifier.set(ownptr, "", valptr);
  }
};

/// @endcond

}
}

#endif //MATADOR_RELATION_FIELD_ENDPOINT_HPP
