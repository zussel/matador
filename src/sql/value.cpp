#include "matador/sql/value.hpp"
#include "matador/sql/token_visitor.hpp"
#include "matador/sql/basic_dialect.hpp"

namespace matador {

value::value()
: token(detail::token::VALUE)
{}

void value::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

}
