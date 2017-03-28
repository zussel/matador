//
// @author sascha on 3/23/17.
//

#ifndef MATADOR_RELATION_FIELD_ENDPOINT_HPP
#define MATADOR_RELATION_FIELD_ENDPOINT_HPP

#include "matador/object/relation_field_serializer.hpp"

#include <string>
#include <functional>
#include <memory>

namespace matador {

class object_proxy;
class prototype_node;
class object_store;

namespace detail {

/// @cond MATADOR_DEV
struct relation_field_endpoint
{
  enum relation_type {
    BELONGS_TO, HAS_ONE, HAS_MANY
  };

  relation_field_endpoint(const std::string &f, relation_type t, prototype_node *pn);
  ~relation_field_endpoint();

  template < class T >
  void set(object_store &store, object_proxy *owner, object_proxy *value);
  template < class T >
  void clear(object_store &store, object_proxy *owner);
  template < class T >
  void append(object_store &store, object_proxy *owner, object_proxy *value);
  template < class T >
  void remove(object_store &store, object_proxy *owner, object_proxy *value);

  std::string field;
  relation_type type;
  prototype_node *node = nullptr;
  std::shared_ptr<relation_field_endpoint> foreign_endpoint;
  bool is_insert_in_progress = false;
  bool is_remove_in_progress = false;

  relation_field_serializer field_serializer;
};

/// @endcond

}
}

#endif //MATADOR_RELATION_FIELD_ENDPOINT_HPP
