#ifndef MATADOR_PERSON_HPP
#define MATADOR_PERSON_HPP

#include "matador/utils/identifier.hpp"
#include "matador/utils/date.hpp"

struct person
{
  matador::identifier<unsigned long> id;
  std::string name;
  matador::date birthday;

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("name", name);
    serializer.serialize("birthday", birthday);
  }
};


#endif //MATADOR_PERSON_HPP
