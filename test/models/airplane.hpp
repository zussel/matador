#ifndef QUERY_AIRPLANE_HPP
#define QUERY_AIRPLANE_HPP

#include "matador/query/mapping/access.hpp"

#include <string>

namespace matador::test {
struct airplane {
  airplane() = default;

  airplane(const unsigned int id, std::string b, std::string m)
  : id(id)
    , brand(std::move(b))
    , model(std::move(m)) {
  }

  unsigned int id{};
  std::string brand;
  std::string model;

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    using namespace matador::utils;
    field::primary_key(op, "id", id);
    field::attribute(op, "brand", brand, VarChar255);
    field::attribute(op, "model", model, VarChar255);
  }
};
}

// namespace matador::access {
// template<class Operator>
// void process(Operator &op, matador::test::airplane &object) {
//     namespace field = matador::access;
//     using namespace matador::utils;
//     field::primary_key(op, "id", object.id);
//     field::attribute(op, "brand", object.brand, UniqueVarChar255);
//     field::attribute(op, "model", object.model, UniqueVarChar255);
// }
// template<class Operator>
// void process(Operator &op, const matador::test::airplane &object) {
//     process(op, const_cast<matador::test::airplane &>(object));
// }
// }
#endif //QUERY_AIRPLANE_HPP
