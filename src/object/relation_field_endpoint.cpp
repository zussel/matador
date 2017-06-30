//
// @author sascha on 3/23/17.
//

//#include <iostream>
//#include <matador/object/object_store.hpp>
//#include "matador/object/relation_field_endpoint.hpp"

#include <matador/object/object_proxy.hpp>

namespace matador {

namespace detail {

void matador::detail::basic_relation_endpoint::insert_value_into_foreign(object_proxy *value, object_proxy *owner)
{
  auto sptr = foreign_endpoint.lock();
  if (sptr) {
    sptr->insert_value(value, owner);
  }
}

void matador::detail::basic_relation_endpoint::remove_value_from_foreign(object_proxy *value, object_proxy *owner)
{
  auto sptr = foreign_endpoint.lock();
  if (sptr) {
    sptr->remove_value(value, owner);
  }
}

}
}