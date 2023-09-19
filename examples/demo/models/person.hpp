#ifndef MATADOR_PERSON_HPP
#define MATADOR_PERSON_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/date.hpp"

#include <utility>

struct person
{
  unsigned long id;
  std::string name;
  matador::date birthday;

  person() = default;
  person(std::string n, matador::date bd)
    : name(std::move(n)), birthday(std::move(bd))
  {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "name", name, 255);
    matador::access::attribute(op, "birthday", birthday);
  }
};


#endif //MATADOR_PERSON_HPP
