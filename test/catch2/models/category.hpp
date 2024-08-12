#ifndef QUERY_CATEGORY_HPP
#define QUERY_CATEGORY_HPP

#include "matador/utils/access.hpp"

#include <string>

namespace matador::test {

struct category
{
  unsigned long id{};
  std::string name;

  template<class Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    using namespace matador::utils;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
  }
};

}
#endif //QUERY_CATEGORY_HPP
