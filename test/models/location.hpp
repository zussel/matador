#ifndef QUERY_LOCATION_HPP
#define QUERY_LOCATION_HPP

#include "matador/query/mapping/access.hpp"

#include "coordinate.hpp"

#include <cstdint>
#include <string>

namespace matador::test {
enum class Color : uint8_t {
  Green, Red, Blue, Yellow, Black, White, Brown
};

struct location {
  unsigned int id{};
  std::string name;
  coordinate coord;
  Color color{Color::Green};

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, UniqueVarChar63);
    field::attribute(op, "coordinate", coord);
    field::attribute(op, "color", color);
  }
};
}

#endif //QUERY_LOCATION_HPP
