#ifndef MATADOR_HANDLER_CREATOR_HPP
#define MATADOR_HANDLER_CREATOR_HPP

namespace matador {

class handler;

class handler_creator
{
public:
  virtual ~handler_creator() = default;

  virtual void notify_close(handler *hndlr) = 0;
};

}
#endif //MATADOR_HANDLER_CREATOR_HPP
