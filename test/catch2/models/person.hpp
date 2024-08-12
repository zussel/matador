#ifndef QUERY_PERSON_HPP
#define QUERY_PERSON_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/types.hpp"

#include <string>

namespace matador::test {

struct person
{
  unsigned long id{};
  std::string name;
  unsigned int age{};
  utils::blob image{};

  template<class Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    using namespace matador::utils;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::attribute(op, "age", age);
    field::attribute(op, "image", image);
  }
};

}
#endif //QUERY_PERSON_HPP
