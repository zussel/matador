#ifndef MATADOR_SOCKET_INTERRUPTER_HPP
#define MATADOR_SOCKET_INTERRUPTER_HPP

#ifdef _MSC_VER
#ifdef matador_net_EXPORTS
#define OOS_NET_API __declspec(dllexport)
#define EXPIMP_NET_TEMPLATE
#else
#define OOS_NET_API __declspec(dllimport)
#define EXPIMP_NET_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_NET_API
#endif

#include "matador/net/ip.hpp"

#include "matador/logger/logger.hpp"

#include <array>

namespace matador {

/// @cond MATADOR_DEV

class OOS_NET_API socket_interrupter
{
public:
  socket_interrupter();
  ~socket_interrupter();

  int socket_id();

  void interrupt();
  bool reset();

private:
  matador::tcp::socket server_;
  matador::tcp::socket client_;

  matador::logger log_;

  std::array<char, 1> indicator_ = { { 0 } };
  std::array<char, 1> consumer_ = {};
};
/// @endcond

}

#endif //MATADOR_SOCKET_INTERRUPTER_HPP
