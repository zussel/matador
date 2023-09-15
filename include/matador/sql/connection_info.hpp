#ifndef MATADOR_CONNECTION_INFO_HPP
#define MATADOR_CONNECTION_INFO_HPP

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

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.on_attribute("type", type);
    serializer.on_attribute("user", user);
    serializer.on_attribute("password", password);
    serializer.on_attribute("hostname", hostname);
    serializer.on_attribute("port", port);
    serializer.on_attribute("database", database);
    serializer.on_attribute("driver", driver);
  }

  static connection_info parse(const std::string &info, unsigned short default_port = 0, const std::string &default_driver = "");
  static std::string to_string(const connection_info &ci);
};

}
#endif //MATADOR_CONNECTION_INFO_HPP
