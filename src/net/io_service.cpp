#include "matador/net/io_service.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {

io_service::io_service()
  : log_(matador::create_logger("IOService"))
{}

void io_service::run()
{
  reactor_.run();
}

}