//
// Created by sascha on 6/13/17.
//

#ifndef MATADOR_TO_MANY_ENDPOINTS_HPP
#define MATADOR_TO_MANY_ENDPOINTS_HPP

#include "matador/object/relation_field_endpoint.hpp"
#include "matador/object/has_one_to_many_item.hpp"
#include "matador/object/has_many_to_many_item.hpp"

namespace matador {
namespace detail {

template < class Value, class Owner >
using has_one_to_many_endpoint = to_many_endpoint<Value, Owner, has_one_to_many_item >;

template < class Value, class Owner >
using has_many_to_many_endpoint = to_many_endpoint<Value, Owner, has_many_to_many_item >;

}
}

#endif //MATADOR_TO_MANY_ENDPOINTS_HPP
