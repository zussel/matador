#ifndef QUERY_FLIGHT_HPP
#define QUERY_FLIGHT_HPP

#include "airplane.hpp"

#include "matador/query/relations/object_ptr.hpp"
#include "matador/query/mapping/access.hpp"

#include <string>
#include <utility>

namespace matador::test {
struct flight {
  flight() = default;

  flight(const unsigned int id, const query::object_ptr<airplane> &plane, std::string name)
  : id(id), plane(plane), pilot_name(std::move(name)) {
  }

  unsigned int id{};
  query::object_ptr<airplane> plane;
  std::string pilot_name;

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    using namespace matador::utils;
    field::primary_key(op, "id", id);
    field::belongs_to(op, "airplane_id", plane, CascadeAllFetchEager);
    field::attribute(op, "pilot_name", pilot_name, VarChar255);
  }
};
}

#endif //QUERY_FLIGHT_HPP
