#ifndef MATADOR_HANDLER_CREATOR_HPP
#define MATADOR_HANDLER_CREATOR_HPP

#include "matador/net/export.hpp"

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
