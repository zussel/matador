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

  template<class Serializer>
  void serialize(Serializer &serializer)
  {
    serializer.on_attribute("alg", algorithm);
    serializer.on_attribute("typ", type);
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

  template<class Serializer>
  void serialize(Serializer &serializer)
  {
    serializer.on_attribute("iss", issuer);
    serializer.on_attribute("sub", subject);
    serializer.on_attribute("aud", audience);
    serializer.on_attribute("exp", expiration_time);
    serializer.on_attribute("nbf", not_before);
    serializer.on_attribute("iat", issued_at);
    serializer.on_attribute("jti", jwt_id);
    serializer.on_attribute("name", name);
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
