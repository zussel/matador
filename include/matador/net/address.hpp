#ifndef MATADOR_ADDRESS_HPP
#define MATADOR_ADDRESS_HPP

#include <string>

#include <netinet/in.h>

namespace matador {

class address
{
public:
  address();
  explicit address(unsigned int addr);
  address(const address &x) = default;

  static address any();
  static address loopback();
  static address broadcast();
  static address from_ip(const std::string &str);
  static address from_ip(const char *str);
  static address from_hostname(const std::string &str);
  static address from_hostname(const char *str);

  unsigned int to_ulong() const;
  std::string to_string() const;

private:
  sockaddr_in addr_ = {};
};

}
#endif //MATADOR_ADDRESS_HPP
