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
    matador::access::primary_key(op, "id", id);
    matador::access::attribute(op, "username", username, 255);
    matador::access::attribute(op, "password", password, 255);
    matador::access::attribute(op, "first_name", first_name, 255);
    matador::access::attribute(op, "last_name", last_name, 255);
  }
};


#endif //MATADOR_USER_HPP
