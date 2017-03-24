//
// @author sascha on 3/23/17.
//

#include <iostream>
#include <matador/object/object_store.hpp>
#include "matador/object/relation_field_endpoint.hpp"

namespace matador {

namespace detail {

relation_field_endpoint::relation_field_endpoint(const std::string &n, relation_type t, prototype_node *pn)
  : name(n), type(t), node(pn)
{
  std::cout << this << " creating relation_field_endpoint (" << this << ")\n";
}

relation_field_endpoint::~relation_field_endpoint()
{
  std::cout << this << " deleting relation_field_endpoint (" << this << ")\n";
}

}
}