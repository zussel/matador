#include "IOServiceTest.hpp"
#include "IOEchoServer.hpp"
#include "NetUtils.hpp"

#include "matador/logger/log_manager.hpp"

#ifndef _WIN32
#include <unistd.h>
#endif

using namespace matador;
using namespace ::detail;

IOServiceTest::IOServiceTest()
  : matador::unit_test("io_service", "io service test unit")
{
  add_test("shutdown", [this] { test_shutdown(); }, "io service shutdown test");
  add_test("acceptor", [this] { test_acceptor(); }, "io service acceptor send and receive test");
  add_test("connector", [this] { test_connector(); }, "io service connector send and receive test");
}

void IOServiceTest::test_shutdown()
{
  IOEchoServer server(7779);
  server.accept();

  server.start();

  std::this_thread::sleep_for(std::chrono::milliseconds (300));

  UNIT_ASSERT_TRUE(utils::wait_until_running(server.service()));

  server.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(server.service()));
}

void IOServiceTest::test_acceptor()
{
  //matador::add_log_sink(matador::create_stdout_sink());
  IOEchoServer server(7780);
  server.accept();

  server.start();

  std::this_thread::sleep_for(std::chrono::milliseconds (300));

  UNIT_ASSERT_TRUE(utils::wait_until_running(server.service()));

  // send and verify received data
  tcp::socket client;

  int ret = client.open(tcp::v4());
  UNIT_ASSERT_FALSE(ret < 0);
  auto srv = tcp::peer(address::v4::loopback(), 7780);
  ret = client.connect(srv);
  UNIT_ASSERT_FALSE(ret < 0);
  buffer data;
  data.append("hallo", 5);
  size_t len = client.send(data);
  UNIT_ASSERT_EQUAL(5UL, len);
  data.clear();
  len = client.receive(data);
  UNIT_ASSERT_EQUAL(5UL, len);
  client.close();

  server.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(server.service()));
}

void IOServiceTest::test_connector()
{
  // setup acceptor
  tcp::acceptor acceptor;

  UNIT_ASSERT_FALSE(acceptor.is_open());
  UNIT_ASSERT_FALSE(acceptor.reuse_address());

  tcp::peer local(address::v4::any(), 7891);
  UNIT_ASSERT_EQUAL(0, acceptor.bind(local));

  UNIT_ASSERT_TRUE(acceptor.reuse_address());
  UNIT_ASSERT_TRUE(acceptor.is_open());
  UNIT_ASSERT_TRUE(acceptor.id() > 0);

  UNIT_ASSERT_EQUAL(0, acceptor.listen(5));

  IOEchoServer server(7891);
  server.connect();

  server.start();

  std::this_thread::sleep_for(std::chrono::milliseconds (300));

  // accept connection
  tcp::socket remote;
  UNIT_ASSERT_TRUE(acceptor.accept(remote) > 0);
  UNIT_ASSERT_TRUE(remote.id() > 0);

  remote.non_blocking(false);

  buffer data;
  data.append("hallo");
  size_t len = remote.send(data);
  UNIT_ASSERT_EQUAL(5UL, len);
  data.clear();

  std::this_thread::sleep_for(std::chrono::seconds (1));

  len = remote.receive(data);
  UNIT_ASSERT_EQUAL(5UL, len);
  remote.close();

  server.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(server.service()));
}
