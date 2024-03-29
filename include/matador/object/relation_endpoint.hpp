#ifndef MATADOR_RELATION_ENDPOINT_HPP
#define MATADOR_RELATION_ENDPOINT_HPP

#include "matador/object/export.hpp"

#include "matador/object/container_item_holder.hpp"
#include "matador/object/object_proxy_accessor.hpp"

namespace matador {

class object_proxy;

class prototype_node;

class object_store;

namespace detail {

/// @cond MATADOR_DEV

/**
 * Basic relation endpoint providing the interface
 */
struct MATADOR_OBJECT_API basic_relation_endpoint : public object_proxy_accessor {
  enum relation_type {
    BELONGS_TO, HAS_ONE, HAS_MANY
  };

  basic_relation_endpoint(std::string fld, prototype_node *n, relation_type t)
  : field(std::move(fld)), node(n), type(t) {
    switch (type) {
      case BELONGS_TO:
        type_name = "belongs_to";
        break;
      case HAS_ONE:
        type_name = "has_one";
        break;
      case HAS_MANY:
        type_name = "container";
        break;
      default:
        break;
    }
  }

  virtual ~basic_relation_endpoint() = default;

  virtual void insert_value(object_proxy *value, object_proxy *owner) = 0;

  virtual void remove_value(object_proxy *value, object_proxy *owner) = 0;

  virtual void insert_value(const basic_container_item_holder &value, object_proxy *owner) = 0;

  virtual void remove_value(const basic_container_item_holder &value, object_proxy *owner) = 0;

  void insert_value_into_foreign(object_proxy *value, object_proxy *owner);

  template<class Value>
  void insert_value_into_foreign(const container_item_holder<Value> &holder, object_proxy *owner);

  void remove_value_from_foreign(object_proxy *value, object_proxy *owner);

  template<class Value>
  void remove_value_from_foreign(const container_item_holder<Value> &holder, object_proxy *owner);

  template<class T>
  void set_has_many_item_proxy(container_item_holder<T> &holder, const object_holder &obj);

  template<class T>
  void set_has_many_item_proxy(container_item_holder<T> &holder, object_proxy *proxy);


  void increment_reference_count(const object_holder &holder);

  void decrement_reference_count(const object_holder &holder);

  void mark_holder_as_inserted(basic_container_item_holder &holder) const;

  void mark_holder_as_removed(basic_container_item_holder &holder) const;

  virtual void print(std::ostream &out) const;

  std::string field;
  std::string type_name;
  prototype_node *node = nullptr;
  relation_type type;
  std::weak_ptr<basic_relation_endpoint> foreign_endpoint;
};

MATADOR_OBJECT_API std::ostream &operator<<(std::ostream &stream, const basic_relation_endpoint &endpoint);

/// @endcond

/**
 * relation endpoint interface with value type
 * @tparam Value
 */
template<class Value>
struct relation_endpoint : public basic_relation_endpoint {
  relation_endpoint(const std::string &field, prototype_node *node, relation_type type)
  : basic_relation_endpoint(field, node, type) {}

  virtual void insert_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) = 0;

  virtual void remove_holder(object_store &store, container_item_holder<Value> &holder, object_proxy *owner) = 0;

  virtual object_proxy *acquire_proxy(unsigned long long oid, object_store &store) = 0;
};

template<class T>
void basic_relation_endpoint::set_has_many_item_proxy(container_item_holder<T> &holder, const object_holder &obj)
{
  set_has_many_item_proxy(holder, obj.proxy_);
}

template<class T>
void basic_relation_endpoint::set_has_many_item_proxy(container_item_holder<T> &holder, object_proxy *proxy)
{
  holder.has_many_to_many_item_poxy_ = proxy;
}

template < class Value >
void basic_relation_endpoint::insert_value_into_foreign(const container_item_holder<Value> &holder, object_proxy *owner)
{
  auto sptr = foreign_endpoint.lock();
  if (sptr) {
    static_cast<relation_endpoint<Value>*>(sptr.get())->insert_value(holder, owner);
  }
}

template < class Value >
void basic_relation_endpoint::remove_value_from_foreign(const container_item_holder<Value> &holder, object_proxy *owner)
{
  auto sptr = foreign_endpoint.lock();
  if (sptr) {
    static_cast<relation_endpoint<Value>*>(sptr.get())->remove_value(holder, owner);
  }
}

}
}

#endif //MATADOR_RELATION_ENDPOINT_HPP
