#ifndef MATADOR_HTTP_CLIENT_HPP
#define MATADOR_HTTP_CLIENT_HPP

#include "matador/http/response.hpp"
#include "matador/http/request.hpp"

#include "matador/net/ip.hpp"
#include "matador/net/io_service.hpp"

#include "matador/logger/logger.hpp"

#include <string>

namespace matador {
namespace http {

class client
{
public:
  explicit client(const std::string &host);

  response get(const std::string &route);
  response post(const std::string &route, const std::string &body);
  response put(const std::string &route, const std::string &body);
  // DELETE
  response remove(const std::string &route);

private:
  void execute();
  void close();
private:
  std::string host_;
  std::string port_ { "80" };

  matador::io_service service_;
  std::shared_ptr<matador::connector> connector_;

  logger log_;

  request request_;
  response response_;
};

}
}

#endif //MATADOR_HTTP_CLIENT_HPP
