//
// @author sascha on 3/23/17.
//

#include <iostream>
#include "matador/object/relation_field_endpoint.hpp"

namespace matador {

namespace detail {

relation_field_endpoint::relation_field_endpoint(const std::string &n,
                                                 relation_type t,
                                                 const modify_value_func &insert_func,
                                                 const modify_value_func &remove_func,
                                                 prototype_node *pn)
  : name(n), type(t), insert_value(insert_func), remove_value(remove_func), node(pn)
{
  std::cout << this << " creating relation_field_endpoint (" << this << ")\n";
}

relation_field_endpoint::~relation_field_endpoint()
{
  std::cout << this << " deleting relation_field_endpoint (" << this << ")\n";
}

void relation_field_endpoint::insert(object_proxy *owner, const std::string &field, object_proxy *value)
{
  if (is_insert_in_progress) {
    return;
  }
  is_insert_in_progress = true;
  insert_value(owner, field, value);
  is_insert_in_progress = false;
}

void relation_field_endpoint::remove(object_proxy *owner, const std::string &field, object_proxy *value)
{
  if (is_remove_in_progress) {
    return;
  }
  is_remove_in_progress = true;
  remove_value(owner, field, value);
  is_remove_in_progress = false;
}

}
}