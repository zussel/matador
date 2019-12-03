//
// Created by sascha on 03.12.19.
//

#ifndef MATADOR_VALUE_VISITOR_HPP
#define MATADOR_VALUE_VISITOR_HPP

#include "matador/utils/any_visitor.hpp"

namespace matador {
namespace detail {

class value_visitor
{
public:
  value_visitor();

  void apply(matador::any &a);


private:
  matador::any_visitor visitor;
};
}
}
#endif //MATADOR_VALUE_VISITOR_HPP
