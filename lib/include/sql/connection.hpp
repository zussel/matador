#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>

namespace oos {

class connection_impl;

class connection
{
public:
  explicit connection(const std::string &dns);

private:
  connection_impl *impl_;
};

}
#endif /* CONNECTION_HPP */
