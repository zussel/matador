#ifndef MATADOR_JWT_TOKEN_HPP
#define MATADOR_JWT_TOKEN_HPP

#include "matador/utils/time.hpp"

namespace matador {
namespace http {
namespace auth {

/// @cond MATADOR_DEV

class jwt_header
{
public:
  std::string algorithm {};
  std::string type {};

  jwt_header() = default;
  jwt_header(std::string algo, std::string typ)
    : algorithm(std::move(algo)), type(std::move(typ))
  {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::attribute(op, "alg", algorithm);
    matador::access::attribute(op, "typ", type);
  }
};

class jwt_payload
{
public:
  std::string issuer {};
  std::string subject {};
  std::string audience {};
  matador::time expiration_time {0};
  matador::time not_before {0};
  matador::time issued_at {0};
  std::string jwt_id {};
  std::string name {};

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::attribute(op, "iss", issuer);
    matador::access::attribute(op, "sub", subject);
    matador::access::attribute(op, "aud", audience);
    matador::access::attribute(op, "exp", expiration_time);
    matador::access::attribute(op, "nbf", not_before);
    matador::access::attribute(op, "iat", issued_at);
    matador::access::attribute(op, "jti", jwt_id);
    matador::access::attribute(op, "name", name);
  }
};

class jwt_token
{
public:
  jwt_header header {};
  jwt_payload payload {};
};

/// @endcond

}
}
}
#endif //MATADOR_JWT_TOKEN_HPP
