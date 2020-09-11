#include "matador/net/io_service.hpp"

namespace matador {
void io_service::run()
{
  reactor_.run();
}

}