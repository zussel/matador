#include "ReactorTest.hpp"
#include "NetUtils.hpp"

#include "matador/net/reactor.hpp"
#include "matador/net/acceptor.hpp"

#include "EchoServer.hpp"

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
  add_test("event_types", std::bind(&ReactorTest::test_event_types, this), "event types test");
  add_test("fdsets", std::bind(&ReactorTest::test_fdset, this), "reactor fdsets test");
  add_test("shutdown", std::bind(&ReactorTest::test_shutdown, this), "reactor shutdown test");
  add_test("send_receive", std::bind(&ReactorTest::test_send_receive, this), "reactor send and receive test");
  add_test("timeout", std::bind(&ReactorTest::test_timeout, this), "reactor schedule timeout test");

}

ReactorTest::~ReactorTest()
{
  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }
}

void ReactorTest::finalize()
{
  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }
}

void ReactorTest::test_event_types()
{
  event_type et = matador::event_type::WRITE_MASK | matador::event_type::READ_MASK;

  UNIT_ASSERT_TRUE(is_event_type_set(et, matador::event_type::WRITE_MASK));
  UNIT_ASSERT_TRUE(is_event_type_set(et, matador::event_type::READ_MASK));
}

void ReactorTest::test_fdset()
{
  reactor r;

  const auto &fds = r.fdsets();

  UNIT_ASSERT_EQUAL(0, fds.maxp1());
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

  worker_thread_ = std::thread([&r] {
    r.run();
    // sleep for some seconds to ensure valid thread join
    std::this_thread::sleep_for(std::chrono::seconds (2));
  });

  UNIT_ASSERT_TRUE(utils::wait_until_running(r));

  r.shutdown();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(r));

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

  worker_thread_ = std::thread([&r] {
    r.run();
    // sleep for some seconds to ensure valid thread join
    std::this_thread::sleep_for(std::chrono::seconds (2));
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

  ac->close();
}

void ReactorTest::test_timeout()
{
  auto echo_conn = std::make_shared<EchoServer>();

  reactor r;

  r.schedule_timer(echo_conn, 1, 1);

  UNIT_ASSERT_EQUAL(1, echo_conn->interval());
  UNIT_ASSERT_GREATER(echo_conn->next_timeout(), 0);

  worker_thread_ = std::thread([&r] {
    r.run();
    // sleep for some seconds to ensure valid thread join
    std::this_thread::sleep_for(std::chrono::seconds (3));
  });

  UNIT_ASSERT_TRUE(utils::wait_until_running(r));

  std::this_thread::sleep_for(std::chrono::seconds (2));

  r.cancel_timer(echo_conn);

  UNIT_ASSERT_EQUAL(0, echo_conn->interval());
  UNIT_ASSERT_EQUAL(0, echo_conn->next_timeout());

  r.shutdown();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(r));
  UNIT_ASSERT_TRUE(echo_conn->timeout_called());
}
