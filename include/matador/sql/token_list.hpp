//
// Created by sascha on 5/31/16.
//

#ifndef OOS_TOKEN_LIST_HPP
#define OOS_TOKEN_LIST_HPP

#include "matador/sql/token.hpp"

#include <list>

namespace matador {

typedef std::shared_ptr<detail::token> token_ptr; /**< Shortcut to a token shared pointer */
typedef std::list<token_ptr> token_list_t;        /**< Shortcut to a list of token shared pointer */

}

#endif //OOS_TOKEN_LIST_HPP
