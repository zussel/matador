#ifndef QUERY_PERSON_HPP
#define QUERY_PERSON_HPP

#include "matador/query/access.hpp"
#include "matador/query/column_options.hpp"

#include "matador/utils/types.hpp"

#include <string>

namespace matador::test {
struct person {
  unsigned int id{};
  std::string name;
  unsigned int age{};
  utils::blob_type_t image{};

  template<class Operator>
  void process(Operator &op) {
    query::field::primary_key(op, "id", id);
    query::field::attribute(op, "name", name, UniqueVarChar255);
    query::field::attribute(op, "age", age);
    query::field::attribute(op, "image", image);
  }
};
}
#endif //QUERY_PERSON_HPP
