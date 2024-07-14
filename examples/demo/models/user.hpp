#ifndef MATADOR_USER_HPP
#define MATADOR_USER_HPP

#include "matador/utils/access.hpp"

#include <string>

struct user
{
  unsigned long id {};
  std::string username {};
  std::string password {};
  std::string first_name {};
  std::string last_name {};

  template < class Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "username", username, 255);
    field::attribute(op, "password", password, 255);
    field::attribute(op, "first_name", first_name, 255);
    field::attribute(op, "last_name", last_name, 255);
  }
};


#endif //MATADOR_USER_HPP
