#ifndef MATADOR_HANDLER_CREATOR_HPP
#define MATADOR_HANDLER_CREATOR_HPP

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

namespace matador {

/// @cond MATADOR_DEV

class handler;

class OOS_NET_API handler_creator
{
public:
  virtual ~handler_creator() = default;

  virtual void notify_close(handler *hndlr) = 0;
};

/// @endcond

}
#endif //MATADOR_HANDLER_CREATOR_HPP
