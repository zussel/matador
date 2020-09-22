#include "ReactorTest.hpp"

#include "matador/net/reactor.hpp"
#include "matador/net/acceptor.hpp"

#include "EchoServer.hpp"

#include <thread>
#include <matador/logger/log_manager.hpp>
#include <utility>

using namespace matador;

ReactorTest::ReactorTest()
  : matador::unit_test("reactor", "reactor test unit")
{
  add_test("shutdown", std::bind(&ReactorTest::test_shutdown, this), "reactor shutdown test");
  add_test("send_receive", std::bind(&ReactorTest::test_send_receive, this), "reactor send and receive test");
}

bool wait_until_running(reactor &r, int retries = 10)
{
  while (!r.is_running() && retries-- > 0) {
    ::usleep(1000);
  }

  return r.is_running();
}

void ReactorTest::test_shutdown()
{
  matador::add_log_sink(matador::create_file_sink("reactor.log"));

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

  UNIT_ASSERT_TRUE(wait_until_running(r));

  r.shutdown();

  server_thread.join();

  ac->close();
}

void ReactorTest::test_send_receive()
{
  matador::add_log_sink(matador::create_file_sink("reactor.log"));

  auto echo_conn = std::make_shared<EchoServer>();

  reactor r;

  auto ep = tcp::peer(address::v4::any(), 7777);
  auto ac = std::make_shared<acceptor>(ep, [echo_conn](tcp::socket sock, tcp::peer p, acceptor *) {
    echo_conn->init(std::move(sock), std::move(p));
    return echo_conn;
  });

  r.register_handler(ac, event_type::ACCEPT_MASK);

  std::thread server_thread([&r] {
    r.run();
  });

  UNIT_ASSERT_TRUE(wait_until_running(r));

  // send and verify received data
  tcp::socket client;

  client.open(tcp::v4());
  auto srv = tcp::peer(address::v4::loopback(), 7777);
  client.connect(srv);

  buffer data;
  data.append("hallo", 5);
  size_t len = client.send(data);
  UNIT_ASSERT_EQUAL(5UL, len);
  data.reset();
  len = client.receive(data);
  UNIT_ASSERT_EQUAL(5UL, len);
  client.close();

  r.shutdown();

  server_thread.join();

  ac->close();
}
