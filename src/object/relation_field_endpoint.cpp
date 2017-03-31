//
// @author sascha on 3/23/17.
//

#include <iostream>
#include <matador/object/object_store.hpp>
#include "matador/object/relation_field_endpoint.hpp"

namespace matador {

namespace detail {

relation_field_endpoint::relation_field_endpoint(const std::string &f, relation_type t, prototype_node *pn)
  : field(f), type(t), node(pn)
{}

relation_field_endpoint::~relation_field_endpoint()
{}

}
}