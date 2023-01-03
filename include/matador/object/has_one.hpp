#ifndef OOS_HAS_ONE_HPP
#define OOS_HAS_ONE_HPP

#include "matador/object/object_ptr.hpp"

namespace matador {

/**
 * Shortcut to object_pointer representing a has_one
 * relationship
 */
template < class T >
using has_one = object_pointer<T, object_holder_type::HAS_ONE>;

}
#endif //OOS_HAS_ONE_HPP
