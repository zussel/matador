//
// Created by sascha on 03.12.19.
//

#include "matador/sql/value_visitor.hpp"

namespace matador {
namespace detail {

value_visitor::value_visitor()
{

}

void value_visitor::apply(matador::any &a)
{
  visitor.visit(a);
}

}
}