#ifndef MATADOR_STATIC_FILE_SERVICE_HPP
#define MATADOR_STATIC_FILE_SERVICE_HPP

#include "matador/http/http_server.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {
namespace http {

class static_file_service
{
public:
  static_file_service(const std::string &path, server &s);

private:
  response serve(const request &req);

private:
  server& server_;

  matador::logger log_;
};

}
}

#endif //MATADOR_STATIC_FILE_SERVICE_HPP
