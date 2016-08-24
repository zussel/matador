//
// Created by sascha on 5/31/16.
//

#ifndef OOS_TOKEN_LIST_HPP
#define OOS_TOKEN_LIST_HPP

#include "sql/token.hpp"

#include <list>

namespace oos {

typedef std::shared_ptr<detail::token> token_ptr;
typedef std::list<token_ptr> token_list_t;

}

#endif //OOS_TOKEN_LIST_HPP
