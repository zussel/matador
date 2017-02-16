//
// Created by sascha on 07.12.15.
//

#ifndef OOS_HAS_ONE_HPP
#define OOS_HAS_ONE_HPP

#include "oos/object/object_ptr.hpp"

namespace oos {

/**
 * Shortcut to object_pointer representing a has_one
 * relationship
 */
template < class T >
using has_one = object_pointer<T, object_holder_type::HAS_ONE>;

}
#endif //OOS_HAS_ONE_HPP
