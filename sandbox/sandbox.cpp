#include "matador/utils/memory.hpp"
#include "matador/logger/basic_file_sink.hpp"
#include "matador/logger/logger.hpp"
#include "matador/logger/log_manager.hpp"
#include "matador/logger/file_sink.hpp"

namespace matador {

class stdout_sink : public basic_file_sink
{
public:
  stdout_sink()
    : basic_file_sink(stdout)
  {}
};

class stderr_sink : public basic_file_sink
{
public:
  stderr_sink()
    : basic_file_sink(stderr)
  {}
};

static log_manager logman;

}

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

matador::logger MyClass::LOG = matador::logman.create_logger("MyClass");

int main()
{
  matador::logman.add(matador::make_unique<matador::file_sink>("log.txt"));
  matador::logman.add(matador::make_unique<matador::stdout_sink>());

  matador::logger log = matador::logman.create_logger("test");

  log.info("hello info %d", 6);
  log.debug("hello debug bug bug bug");
  log.error("hello error -> fatal");
  log.warn("hello warn: attention");
  log.trace("hello trace: what happens here");

  MyClass my1;
  MyClass my2;
}
