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
struct MATADOR_OBJECT_API relation_field_endpoint
{
  enum relation_type {
    BELONGS_TO, HAS_ONE, HAS_MANY
  };

  enum has_many_relation_side {
    LEFT, RIGHT
  };
  relation_field_endpoint(const std::string &f, relation_type t, prototype_node *pn);
  ~relation_field_endpoint();

  template < class T >
  void set(object_store &store, const object_ptr<T> &owner, object_proxy *value);
  template < class T >
  void clear(object_store &store, const object_ptr<T> &owner);
  template < class T >
  void append(object_store &store, const object_ptr<T> &owner, object_proxy *value);
  template < class T >
  void remove(object_store &store, const object_ptr<T> &owner, object_proxy *value);

  template < class T >
  void create(object_store &store, const object_ptr<T> &owner, object_store *value);

  template < class T, class V >
  void insert(object_store &store, const object_ptr<T> &value, object_proxy *owner);

  std::string field;
  relation_type type;
  has_many_relation_side side = LEFT;
  prototype_node *node = nullptr;
  std::weak_ptr<relation_field_endpoint> foreign_endpoint;
  bool is_insert_in_progress = false;
  bool is_remove_in_progress = false;

  relation_field_serializer field_serializer;
};

template < class Endpoint >
struct relation_endpoint
{

};

/*
 * one_to_many endpoint
 *
 * foreign endpoint is a many_to_one endpoint
 */
template < class T >
struct one_to_many_endpoint : relation_endpoint<one_to_many_endpoint<T>>
{
  typedef T value_type;

  using insert_func_type = std::function<has_many_item_holder<T>*(object_store &, const object_ptr<T> &,object_proxy *)>;

  insert_func_type insert_func;

  void insert_object(object_store &store, const object_ptr<T> &owner, object_proxy *value)
  {

  }
};
/*
 * many_to_one endpoint
 *
 *
 * one_to_one endpoint
 */
template < class T >
struct rfe {

};
/// @endcond

}
}

#endif //MATADOR_RELATION_FIELD_ENDPOINT_HPP
