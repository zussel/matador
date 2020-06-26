#include "matador/net/handler.hpp"

namespace matador {

reactor *handler::get_reactor() const
{
  return reactor_;
}

void handler::register_reactor(reactor *r)
{
  reactor_ = r;
}

}
