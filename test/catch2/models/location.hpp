#ifndef QUERY_LOCATION_HPP
#define QUERY_LOCATION_HPP

#include "matador/utils/access.hpp"

#include "coordinate.hpp"

#include <cstdint>

namespace matador::test {

enum class Color : uint8_t {
  Green, Red, Blue, Yellow, Black, White, Brown
};

struct location
{
  unsigned long id{};
  std::string name;
  coordinate coord;
  Color color{Color::Green};

  template < class Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::attribute(op, "coordinate", coord);
    field::attribute(op, "color", color);
  }
};

}

#endif //QUERY_LOCATION_HPP
