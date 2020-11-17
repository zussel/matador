#ifndef MATADOR_HTTP_CLIENT_HPP
#define MATADOR_HTTP_CLIENT_HPP

#include <string>

namespace matador {

class http_client
{
public:
  void get(const std::string &route);
};

}

#endif //MATADOR_HTTP_CLIENT_HPP
