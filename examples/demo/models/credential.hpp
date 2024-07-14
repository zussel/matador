#ifndef MATADOR_CREDENTIAL_HPP
#define MATADOR_CREDENTIAL_HPP

#include "matador/utils/access.hpp"

#include <string>

struct credential
{
  std::string username {};
  std::string password {};

  template < class Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::attribute(op, "username", username, 255);
    field::attribute(op, "password", password, 255);
  }
};

#endif //MATADOR_CREDENTIAL_HPP
