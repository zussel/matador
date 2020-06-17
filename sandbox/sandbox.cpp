#include <thread>
#include "matador/logger/logger.hpp"
#include "matador/logger/log_manager.hpp"

class MyClass
{
public:
  static matador::logger LOG;

  MyClass() {
    LOG.info("created");
  }

  ~MyClass()
  {
    LOG.info("destroyed");
  }
};

matador::logger MyClass::LOG = matador::create_logger("MyClass");

int main()
{
  std::thread::id this_id = std::this_thread::get_id();

  auto logsink = matador::create_file_sink("log.txt");
  auto stdoutsink = matador::create_stdout_sink();

  matador::add_log_sink(stdoutsink);
  matador::add_log_sink(logsink);

  auto log = matador::create_logger("test");

  log.info("hello info %d [%d]", 6, this_id);
  log.debug("hello debug bug bug bug");
  log.error("hello error -> fatal");
  log.warn("hello warn: attention");
  log.trace("hello trace: what happens here");

  MyClass my1;
  MyClass my2;

  auto netsink = matador::create_file_sink("netlog.txt");
  matador::add_log_sink(netsink, "net");

  auto another_log = matador::create_logger("side", "net");

  another_log.warn("another net log");
}
