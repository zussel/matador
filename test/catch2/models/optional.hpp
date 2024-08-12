#ifndef QUERY_OPTIONAL_HPP
#define QUERY_OPTIONAL_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"

#include <optional>
#include <string>

namespace matador::test {

struct optional
{
  unsigned long id{};
  std::optional<std::string> name;
  std::optional<unsigned int> age{};

  template<class Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    using namespace matador::utils;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::attribute(op, "age", age);
  }
};

}

#endif //QUERY_OPTIONAL_HPP
