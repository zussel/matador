#ifndef MATADOR_ADDRESS_HPP
#define MATADOR_ADDRESS_HPP

#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

namespace matador {

enum protocol_family {
  V4, V6
};

template < protocol_family pf >
class address_router;

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

  static const address_router<V4> v4;
  static const address_router<V6> v6;

private:
  sockaddr_in addr_ = {};
};

template <>
class address_router<V4>
{
public:
  address_router() = default;
  address_router& operator=(const address_router&) = delete;
  address_router(const address_router&) = delete;
  address_router& operator=(address_router&&) = delete;
  address_router(address_router&&) = delete;

  address any() { return address(static_cast<unsigned int>(INADDR_ANY)); }
  address loopback() { return address(static_cast<unsigned int>(INADDR_LOOPBACK)); }
  address broadcast() {return address(static_cast<unsigned int>(INADDR_BROADCAST)); }
//  address from_ip(const std::string &str);
//  address from_ip(const char *str);
//  address from_hostname(const std::string &str);
//  address from_hostname(const char *str);

};

template <>
class address_router<V6>
{
public:
  address_router() = default;
  address_router& operator=(const address_router&) = delete;
  address_router(const address_router&) = delete;
  address_router& operator=(address_router&&) = delete;
  address_router(address_router&&) = delete;

  address any() { return address(static_cast<unsigned int>(INADDR_ANY)); }
  address loopback() { return address(static_cast<unsigned int>(INADDR_LOOPBACK)); }
  address broadcast() {return address(static_cast<unsigned int>(INADDR_BROADCAST)); }
//  address from_ip(const std::string &str);
//  address from_ip(const char *str);
//  address from_hostname(const std::string &str);
//  address from_hostname(const char *str);

};


}
#endif //MATADOR_ADDRESS_HPP
