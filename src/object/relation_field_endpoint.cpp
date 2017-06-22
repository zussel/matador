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
  std::cout << "insert_value_into_foreign (proxy) owner " << owner->id() << " (type: " << owner->classname() << ")\n";
  if (is_inserting) {
    return;
  }

  is_inserting = true;

  auto sptr = foreign_endpoint.lock();
  if (sptr) {
    sptr->insert_value(value, owner);
  }

  is_inserting = false;
}

void matador::detail::basic_relation_endpoint::remove_value_from_foreign(object_proxy *value, object_proxy *owner)
{
  if (is_removing) {
    return;
  }

  is_removing = true;

  auto sptr = foreign_endpoint.lock();
  if (sptr) {
    sptr->remove_value(value, owner);
  }

  is_removing = false;
}

}
}