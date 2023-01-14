#ifndef MATADOR_ACTION_VECTOR_HPP
#define MATADOR_ACTION_VECTOR_HPP

#include "matador/object/action.hpp"

#include <memory>
#include <vector>

namespace matador {

typedef std::shared_ptr<action> action_ptr;      /**< Shortcut to an action shared pointer */
typedef std::vector<action_ptr> t_action_vector; /**< Shortcut to a vector of action shared pointer */

}

#endif //MATADOR_ACTION_VECTOR_HPP
