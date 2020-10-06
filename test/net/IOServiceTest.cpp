#include "IOServiceTest.hpp"
#include "IOEchoServer.hpp"

#include "matador/net/io_service.hpp"

#include "matador/logger/log_manager.hpp"

#include <thread>

#ifndef _WIN32
#include <unistd.h>
#endif

using namespace matador;

IOServiceTest::IOServiceTest()
  : matador::unit_test("io_service", "io service test unit")
{
  add_test("service", std::bind(&IOServiceTest::test_service, this), "io service test");
}

bool wait_until_running(io_service &srv, int retries = 10)
{
  while (!srv.is_running() && retries-- > 0) {
#ifdef _WIN32
    ::Sleep(1000);
#else
    ::usleep(500);
#endif
  }

  return srv.is_running();
}

void IOServiceTest::test_service()
{
  auto logsink = matador::create_file_sink("io_service.log");
  matador::add_log_sink(logsink);

  IOEchoServer server(7777);

  std::thread server_thread([&server] {
    server.run();
  });

  UNIT_ASSERT_TRUE(wait_until_running(server.service()));

  server.service().shutdown();

  server_thread.join();
}
