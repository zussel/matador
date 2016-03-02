//
// Created by sascha on 2/26/16.
//

#include "sql/token.hpp"

namespace oos {
namespace detail {

token::token(basic_dialect::t_token tok)
  : type(tok)
{}

}
}