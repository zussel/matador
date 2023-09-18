#ifndef MATADOR_CONNECTION_INFO_HPP
#define MATADOR_CONNECTION_INFO_HPP

#include "matador/utils/access.hpp"

#include <string>

namespace matador {

struct connection_info
{
  std::string type;
  std::string user;
  std::string password;
  std::string hostname;
  unsigned short port{};
  std::string database;
  std::string driver;

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::attribute(op, "type", type);
    matador::access::attribute(op, "user", user);
    matador::access::attribute(op, "password", password);
    matador::access::attribute(op, "hostname", hostname);
    matador::access::attribute(op, "port", port);
    matador::access::attribute(op, "database", database);
    matador::access::attribute(op, "driver", driver);
  }

  static connection_info parse(const std::string &info, unsigned short default_port = 0, const std::string &default_driver = "");
  static std::string to_string(const connection_info &ci);
};

}
#endif //MATADOR_CONNECTION_INFO_HPP
