#ifndef MATADOR_MOVIE_SERVICE_HPP
#define MATADOR_MOVIE_SERVICE_HPP

#include "matador/logger/logger.hpp"

#include "matador/http/response.hpp"

namespace matador {
class persistence;

namespace http {

class server;
class request;

}
}

class movie_service
{
public:
  movie_service(matador::http::server &s, matador::persistence &p);

  matador::http::response initialize(const matador::http::request &p);
  matador::http::response list(const matador::http::request &p);
  matador::http::response get_movie(const matador::http::request &p);
  matador::http::response create_movie(const matador::http::request &p);
  matador::http::response update_movie(const matador::http::request &p);
  matador::http::response delete_movie(const matador::http::request &p);

private:
  matador::logger log_;

  matador::persistence& persistence_;
};


#endif //MATADOR_MOVIE_SERVICE_HPP
