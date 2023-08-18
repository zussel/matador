#ifndef MATADOR_PERSON_HPP
#define MATADOR_PERSON_HPP

#include <utility>

#include "matador/utils/date.hpp"

struct person
{
  unsigned long id;
  std::string name;
  matador::date birthday;

  person() = default;
  person(std::string n, matador::date bd)
    : name(std::move(n)), birthday(std::move(bd))
  {}

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.on_primary_key("id", id);
    serializer.on_attribute("name", name, 255);
    serializer.on_attribute("birthday", birthday);
  }
};


#endif //MATADOR_PERSON_HPP
