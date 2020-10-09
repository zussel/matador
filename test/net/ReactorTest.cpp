#include "ReactorTest.hpp"
#include "NetUtils.hpp"

#include "matador/net/reactor.hpp"
#include "matador/net/acceptor.hpp"

#include "EchoServer.hpp"

#include "matador/logger/log_manager.hpp"

#include <thread>
#include <utility>

#ifndef _WIN32
#include <unistd.h>
#endif

using namespace matador;
using namespace ::detail;

ReactorTest::ReactorTest()
  : matador::unit_test("reactor", "reactor test unit")
{
  add_test("shutdown", std::bind(&ReactorTest::test_shutdown, this), "reactor shutdown test");
  add_test("send_receive", std::bind(&ReactorTest::test_send_receive, this), "reactor send and receive test");
}


void ReactorTest::test_shutdown()
{
  reactor r;

  auto ep = tcp::peer(address::v4::any(), 7777);
  auto ac = std::make_shared<acceptor>(ep, [](tcp::socket sock, tcp::peer p, acceptor *) {
    auto cl = std::make_shared<EchoServer>();
    cl->init(std::move(sock), std::move(p));
    return cl;
  });

  r.register_handler(ac, event_type::ACCEPT_MASK);

  std::thread server_thread([&r] {
    r.run();
  });

  UNIT_ASSERT_TRUE(utils::wait_until_running(r));

  r.shutdown();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(r));

  server_thread.join();

  ac->close();
}

void ReactorTest::test_send_receive()
{
  auto echo_conn = std::make_shared<EchoServer>();

  reactor r;

  auto ep = tcp::peer(address::v4::any(), 7778);
  auto ac = std::make_shared<acceptor>(ep, [echo_conn](tcp::socket sock, tcp::peer p, acceptor *) {
    echo_conn->init(std::move(sock), std::move(p));
    return echo_conn;
  });

  r.register_handler(ac, event_type::ACCEPT_MASK);

  std::thread server_thread([&r] {
    r.run();
  });

  UNIT_ASSERT_TRUE(utils::wait_until_running(r));

  // send and verify received data
  tcp::socket client;

  int ret = client.open(tcp::v4());
  UNIT_ASSERT_FALSE(ret < 0);
  auto srv = tcp::peer(address::v4::loopback(), 7778);
  ret = client.connect(srv);
  UNIT_ASSERT_FALSE(ret < 0);
  buffer data;
  data.append("hallo");
  size_t len = client.send(data);
  UNIT_ASSERT_EQUAL(5UL, len);
  data.clear();
  len = client.receive(data);
  UNIT_ASSERT_EQUAL(5UL, len);
  client.close();

  r.shutdown();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(r));

  server_thread.join();

  ac->close();
}
