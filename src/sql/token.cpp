#include "matador/sql/token.hpp"

#include <utility>

namespace matador::detail {

token::token(t_token tok)
  : type(tok)
{}

table_name_token::table_name_token(token::t_token tok, std::string table_name)
  : token(tok)
  , table_name(std::move(table_name))
{}

}
