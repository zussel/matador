#include "matador/net/handler.hpp"

namespace matador {

void handler::register_reactor(reactor *r)
{
  reactor_ = r;
}

}
