//
// @author sascha on 3/23/17.
//

#ifndef MATADOR_RELATION_FIELD_ENDPOINT_HPP
#define MATADOR_RELATION_FIELD_ENDPOINT_HPP

#include <string>
#include <functional>
#include <memory>

namespace matador {

class object_proxy;
class prototype_node;

namespace detail {

/// @cond MATADOR_DEV
struct relation_field_endpoint
{
  enum relation_type {
    BELONGS_TO, HAS_ONE, HAS_MANY
  };

  typedef std::function<void(object_proxy*, const std::string&, object_proxy*)> modify_value_func;

  relation_field_endpoint(const std::string &n,
                          relation_type t,
                          const modify_value_func &insert_func,
                          const modify_value_func &remove_func,
                          prototype_node *pn);

  ~relation_field_endpoint();

  void insert(object_proxy *owner, const std::string &field, object_proxy *value);
  void remove(object_proxy *owner, const std::string &field, object_proxy *value);

  std::string name;
  relation_type type;
  std::function<void(object_proxy*, const std::string&, object_proxy*)> insert_value;
  std::function<void(object_proxy*, const std::string&, object_proxy*)> remove_value;
  prototype_node *node = nullptr;
  std::shared_ptr<relation_field_endpoint> foreign_endpoint;
  bool is_insert_in_progress = false;
  bool is_remove_in_progress = false;
};

/// @endcond

}
}

#endif //MATADOR_RELATION_FIELD_ENDPOINT_HPP
