#include "IOServiceTest.hpp"
#include "IOEchoServer.hpp"
#include "NetUtils.hpp"

#include "matador/net/io_service.hpp"

#include "matador/logger/log_manager.hpp"

#include <thread>

#ifndef _WIN32
#include <unistd.h>
#endif

using namespace matador;
using namespace ::detail;

IOServiceTest::IOServiceTest()
  : matador::unit_test("io_service", "io service test unit")
{
  add_test("service", std::bind(&IOServiceTest::test_service, this), "io service test");
}

void IOServiceTest::test_service()
{
  auto logsink = matador::create_file_sink("io_service.log");
  matador::add_log_sink(logsink);

  IOEchoServer server(7777);

  std::thread server_thread([&server] {
    server.run();
  });

  UNIT_ASSERT_TRUE(utils::wait_until_running(server.service()));

  server.service().shutdown();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(server.service()));

  if (server_thread.joinable()) {
    server_thread.join();
  }
}
