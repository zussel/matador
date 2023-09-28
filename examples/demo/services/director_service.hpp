#ifndef MATADOR_DIRECTOR_SERVICE_HPP
#define MATADOR_DIRECTOR_SERVICE_HPP

#include "matador/logger/logger.hpp"

#include "matador/http/response.hpp"

namespace matador {
class persistence;

namespace http {

class server;
class request;

}
}

class director_service
{
public:
  director_service(matador::http::server &s, matador::persistence &p);

  matador::http::response list(const matador::http::request &p);
  matador::http::response get(const matador::http::request &p);
  matador::http::response create(const matador::http::request &p);
  matador::http::response update(const matador::http::request &p);
  matador::http::response remove(const matador::http::request &p);

private:
  matador::logger log_;

  matador::persistence& persistence_;
};


#endif //MATADOR_DIRECTOR_SERVICE_HPP
