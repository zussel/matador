#ifndef QUERY_AIRPLANE_HPP
#define QUERY_AIRPLANE_HPP

#include "category.hpp"
#include "supplier.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/field_attributes.hpp"

#include <string>

namespace matador::test {

struct airplane
{
  airplane() = default;
  airplane(const unsigned long id, std::string b, std::string m)
  : id(id)
  , brand(std::move(b))
  , model(std::move(m)) {}
  unsigned long id{};
  std::string brand;
  std::string model;

  template<class Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    using namespace matador::utils;
    field::primary_key(op, "id", id);
    field::attribute(op, "brand", brand, 255);
    field::attribute(op, "model", model, 255);
  }
};

}

#endif //QUERY_AIRPLANE_HPP
