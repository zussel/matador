#include "matador/net/io_service.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {

io_service::io_service()
  : log_(matador::create_logger("IOService"))
{}

io_service::~io_service()
{
  reactor_.shutdown();
}

void io_service::run()
{
  reactor_.run();
}

bool io_service::is_running() const
{
  return reactor_.is_running();
}

void io_service::shutdown()
{
  reactor_.shutdown();
}

}