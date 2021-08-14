#ifndef MATADOR_AUTH_SERVICE_HPP
#define MATADOR_AUTH_SERVICE_HPP

#include "matador/http/response.hpp"

#include "matador/logger/logger.hpp"

namespace matador {
namespace http {

class server;
class request;

}
}

class auth_service
{
public:
  explicit auth_service(matador::http::server &s);

  matador::http::response login(const matador::http::request &req);
  matador::http::response logout(const matador::http::request &req);

private:
  matador::logger log_;
};


#endif //MATADOR_AUTH_SERVICE_HPP
