#include "sql/connection_factory.hpp"
#include "sql/connection.hpp"

namespace oos {

connection::connection(const std::string &dns)
{
  // parse dbstring
  std::string::size_type pos = dns.find(':');
  type_ = dns.substr(0, pos);
  dns_ = dns.substr(pos + 3);

  // get driver factory singleton
  connection_factory &df = connection_factory::instance();

  // try to create database implementation
  impl_ = df.create(type_);
}

}