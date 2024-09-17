#ifndef QUERY_FLIGHT_HPP
#define QUERY_FLIGHT_HPP

#include "airplane.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/fetch_type.hpp"
#include "matador/utils/field_attributes.hpp"

#include "matador/object/object_ptr.hpp"

#include <string>
#include <utility>

namespace matador::test {

struct flight
{
  flight() = default;
  flight(const unsigned long id, const object_ptr<airplane> &plane, std::string name)
  : id(id), airplane(plane), pilot_name(std::move(name)) {}

  unsigned long id{};
  object_ptr<airplane> airplane;
  std::string pilot_name;

  template<class Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    using namespace matador::utils;
    field::primary_key(op, "id", id);
    field::has_one(op, "airplane_id", airplane, {utils::cascade_type::ALL, utils::fetch_type::EAGER});
    field::attribute(op, "pilot_name", pilot_name, 255);
  }
};

}

#endif //QUERY_FLIGHT_HPP
