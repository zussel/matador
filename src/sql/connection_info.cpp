#include "matador/sql/connection_info.hpp"

#include <regex>

namespace matador {

connection_info connection_info::parse(const std::string &info, unsigned short default_port, const std::string &default_driver) {
  static const std::regex DNS_RGX (R"((\w+):\/\/((([\w]+)(:([\w!]+))?)@)?((((([\w.]+)([:]([\d]+))?)?)\/)?(([\w.]+)( \((.*)\))?)))");

  std::smatch what;

  if (!std::regex_match(info, what, DNS_RGX)) {
    throw std::logic_error("connect invalid dns: " + info);
  }

  connection_info ci{};
  ci.type = what[1].str();
  ci.user = what[4].str();
  ci.password = what[6].str();

  // get connection part
  ci.hostname = what[11].str();
  if (what[13].matched) {
    char *end;
    ci.port = static_cast<unsigned short>(std::strtoul(what[13].str().c_str(), &end, 10));
  } else {
    ci.port = default_port;
  }

  // Should we just ignore the "instance" part?
  // const std::string instance = what[5].str();
  ci.database = what[15].str();

  if (what[17].matched) {
    ci.driver = what[17].str();
  } else {
    ci.driver = default_driver;
  }

  return ci;
}

std::string connection_info::to_string(const connection_info &ci) {
  std::string dns{ci.type};
  dns += "://";

  if (!ci.user.empty()) {
    dns += ci.user;
    if (!ci.password.empty()) {
      dns += ":" + ci.password;
    }
  }

  if (!ci.hostname.empty()) {
    dns += "@" + ci.hostname;
    if (ci.port > 0) {
      dns += ":" + std::to_string(ci.port);
    }
  }

  if (!dns.empty()) {
    dns += "/";
  }
  dns += ci.database;

  if (!ci.driver.empty()) {
    dns += " (" + ci.driver +")";
  }
  return dns;
}

}