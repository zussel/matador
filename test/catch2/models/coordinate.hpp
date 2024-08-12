#ifndef QUERY_COORDINATE_HPP
#define QUERY_COORDINATE_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"

namespace matador::test {
struct coordinate
{
  int x{};
  int y{};
  int z{};
};

}

namespace matador::access {

template<class Operator>
void attribute(Operator &op, const char *id, test::coordinate &value, const utils::field_attributes &attr = utils::null_attributes) {
  attribute(op, (std::string(id) + "_x").c_str(), value.x, attr);
  attribute(op, (std::string(id) + "_y").c_str(), value.y, attr);
  attribute(op, (std::string(id) + "_z").c_str(), value.z, attr);
}

}

#endif //QUERY_COORDINATE_HPP
