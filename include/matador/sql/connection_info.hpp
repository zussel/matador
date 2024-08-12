#ifndef QUERY_CONNECTION_INFO_HPP
#define QUERY_CONNECTION_INFO_HPP

#include <string>

namespace matador::sql {
struct connection_info {
  std::string type;
  std::string user;
  std::string password;
  std::string hostname;
  unsigned short port{};
  std::string database;
  std::string driver;

  static connection_info parse(const std::string &info,
                               unsigned short default_port = 0,
                               const std::string &default_driver = "");
  static std::string to_string(const connection_info &ci);
};
}
#endif //QUERY_CONNECTION_INFO_HPP
