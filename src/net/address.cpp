#include "matador/net/address.hpp"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#include <libnet.h>
#endif

#include <cstring>
#include <stdexcept>

namespace matador {

address_router address::v4 = address_router(IN)
address::address()
{
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = PF_INET;
}

address::address(unsigned int addr)
{
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = PF_INET;
  addr_.sin_addr.s_addr = htonl(addr);
}

address address::any()
{
  return address(static_cast<unsigned int>(INADDR_ANY));
}

address address::loopback()
{
  return address(static_cast<unsigned int>(INADDR_LOOPBACK));
}

address address::broadcast()
{
  return address(static_cast<unsigned int>(INADDR_BROADCAST));
}

address address::from_ip(const std::string &str)
{
  return address::from_ip(str.c_str());
}

address address::from_ip(const char *str)
{
  // now fill in the address info struct
  // create and fill the hints struct
  if (str == nullptr) {
    return address();
  }
  // get address from string
  address tmp;
  inet_pton(PF_INET, str, &(tmp.addr_.sin_addr));

  /*
  unsigned long ip = inet_addr(str);
  if (ip != INADDR_NONE) {
    tmp.addr_.sin_addr.s_addr = ip;
  } else {
	  struct hostent *he;
	  if ((he=gethostbyname(str)) == NULL) {  // get the host info
      return address();
      //throw new std::exception("error: gethostbyname failed", WSAGetLastError());
	  }
	  tmp.addr_.sin_addr = *((struct in_addr *)he->h_addr);
  }
  tmp.addr_.sin_family = PF_INET;
  memset(tmp.addr_.sin_zero, '\0', sizeof(tmp.addr_.sin_zero));
  */
  return tmp;
}

address address::from_hostname(const std::string &str)
{
  return address::from_hostname(str.c_str());
}

address address::from_hostname(const char *str)
{
  // now fill in the address info struct
  // create and fill the hints struct
  if (str == nullptr) {
    return address();
  }
  // get address from string
  address tmp;
  unsigned int ip = inet_addr(str);
  if (ip != INADDR_NONE) {
    tmp.addr_.sin_addr.s_addr = ip;
  } else {
    struct hostent *he;
    if ((he=gethostbyname(str)) == nullptr) {  // get the host info
      return address();
      //throw new std::exception("error: gethostbyname failed", WSAGetLastError());
    }
    tmp.addr_.sin_addr = *((struct in_addr *)he->h_addr);
  }
  tmp.addr_.sin_family = PF_INET;
  memset(tmp.addr_.sin_zero, '\0', sizeof(tmp.addr_.sin_zero));
  return tmp;
}

unsigned int address::to_ulong() const
{
  return addr_.sin_addr.s_addr;
//  return ntohl(addr_.sin_addr.s_addr);
}

std::string address::to_string() const
{
  char addstr[INET_ADDRSTRLEN];
  const char *str = inet_ntop(addr_.sin_family, &(addr_.sin_addr), addstr, INET_ADDRSTRLEN);
  if (str == nullptr) {
    throw std::logic_error("inet_ntop error");
  }
  return str;

//  return inet_ntoa(addr_.sin_addr);
}

}
