#include "ReactorTest.hpp"

#include "../NetUtils.hpp"

#include "matador/net/reactor.hpp"
#include "matador/net/acceptor.hpp"
#include "matador/net/connector.hpp"

#include "matador/logger/log_manager.hpp"

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
  add_test("event_types", [this] { test_event_types(); }, "event types test");
  add_test("fdsets", [this] { test_fdset(); }, "reactor fdsets test");
  add_test("handler", [this] { test_handler(); }, "reactor handler test");
  add_test("connector", [this] { test_connector(); }, "connector test");
  add_test("shutdown", [this] { test_shutdown(); }, "reactor shutdown test");
  add_test("reactor_acceptor", [this] { test_reactor_acceptor(); }, "reactor acceptor send and receive test");
  add_test("reactor_connector", [this] { test_reactor_connector(); }, "reactor connector send and receive test");
  add_test("timeout", [this] { test_timeout(); }, "reactor schedule timeout test");

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

  auto fds = r.fdsets();

  UNIT_ASSERT_GREATER(fds.maxp1(), 0);
}

void ReactorTest::test_handler()
{
  reactor r;

  auto h = std::make_shared<EchoServer>();

  r.register_handler(h, event_type::ALL_MASK);

  auto it = r.find_handler_type(h);
  UNIT_ASSERT_NOT_NULL(it->first.get());

  UNIT_ASSERT_TRUE(is_event_type_set(it->second, event_type::READ_MASK));
  UNIT_ASSERT_TRUE(is_event_type_set(it->second, event_type::WRITE_MASK));
  UNIT_ASSERT_TRUE(is_event_type_set(it->second, event_type::ACCEPT_MASK));
  UNIT_ASSERT_TRUE(is_event_type_set(it->second, event_type::EXCEPT_MASK));

  r.deactivate_handler(h, event_type::READ_MASK);

  UNIT_ASSERT_FALSE(is_event_type_set(it->second, event_type::READ_MASK));

  r.activate_handler(h, event_type::READ_MASK);

  UNIT_ASSERT_TRUE(is_event_type_set(it->second, event_type::READ_MASK));
}

void ReactorTest::test_connector()
{
  connector c;

  UNIT_ASSERT_EQUAL("connector", c.name());
  UNIT_ASSERT_EQUAL(0, c.handle());

  c.open();

  UNIT_ASSERT_EQUAL(0, c.handle());

  c.close();

  UNIT_ASSERT_EQUAL(0, c.handle());
}

void ReactorTest::test_shutdown()
{
//  matador::default_min_log_level(log_level::LVL_DEBUG);
//  matador::add_log_sink(matador::create_stdout_sink());

  auto ep = tcp::peer(address::v4::any(), 7777);
  auto ac = std::make_shared<acceptor>(ep, [](tcp::socket sock, tcp::peer p, acceptor *) {
    auto cl = std::make_shared<EchoServer>();
    cl->init(std::move(sock), std::move(p));
    return cl;
  });

  reactor r;
  r.register_handler(ac, matador::event_type::ACCEPT_MASK);

  utils::ThreadRunner runner([&r]() {
    r.run();
  }, [&r]() {
    r.shutdown();
  });

  std::this_thread::sleep_for(std::chrono::milliseconds (300));

  UNIT_ASSERT_TRUE(utils::wait_until_running(r));

  r.shutdown();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(r));
}

void ReactorTest::test_reactor_acceptor()
{
  auto echo_conn = std::make_shared<EchoServer>();

  auto ep = tcp::peer(address::v4::any(), 7779);
  auto ac = std::make_shared<acceptor>(ep, [echo_conn](tcp::socket sock, tcp::peer p, acceptor *) {
    echo_conn->init(std::move(sock), std::move(p));
    return echo_conn;
  });

  UNIT_ASSERT_EQUAL( "acceptor", ac->name() );

  reactor r;
  r.register_handler(ac, event_type::ACCEPT_MASK);

  utils::ThreadRunner runner([&r]() {
    r.run();
  }, [&r]() {
    r.shutdown();
  });

  std::this_thread::sleep_for(std::chrono::milliseconds (800));

  UNIT_ASSERT_TRUE(utils::wait_until_running(r));

  // send and verify received data
  tcp::socket client;

  auto ret = client.open(tcp::v4());
  UNIT_ASSERT_FALSE(ret < 0);
  auto srv = tcp::peer(address::v4::loopback(), 7779);
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
}

void ReactorTest::test_reactor_connector()
{
  // setup acceptor
  tcp::acceptor acceptor;

  UNIT_ASSERT_FALSE(acceptor.is_open());
  UNIT_ASSERT_FALSE(acceptor.reuse_address());

  tcp::peer local(address::v4::any(), 7890);
  UNIT_ASSERT_EQUAL(0, acceptor.bind(local));

  UNIT_ASSERT_TRUE(acceptor.reuse_address());
  UNIT_ASSERT_TRUE(acceptor.is_open());
  UNIT_ASSERT_TRUE(acceptor.id() > 0);

  UNIT_ASSERT_EQUAL(0, acceptor.listen(5));

  // setup connector in reactor wrapper
  auto echo_conn = std::make_shared<EchoServer>();

  auto echo_connector = std::make_shared<connector>([echo_conn](const tcp::socket& sock, const tcp::peer &p, connector *) {
    echo_conn->init(sock, p);
    return echo_conn;
  });

  reactor r;
  tcp::resolver resolver;
  auto endpoints = resolver.resolve("localhost", std::to_string(7890));
  echo_connector->connect(r, endpoints);

  utils::ThreadRunner runner([&r]() {
    r.run();
  }, [&r]() {
    r.shutdown();
  });

  std::this_thread::sleep_for(std::chrono::milliseconds (100));

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

  std::this_thread::sleep_for(std::chrono::milliseconds (300));

  len = remote.receive(data);
  UNIT_ASSERT_EQUAL(5UL, len);
  remote.close();

  r.shutdown();
}

void ReactorTest::test_timeout()
{
  auto echo_conn = std::make_shared<EchoServer>();

  reactor r;
  r.schedule_timer(echo_conn, 1, 1);

  UNIT_ASSERT_EQUAL(1, echo_conn->interval());
  UNIT_ASSERT_GREATER(echo_conn->next_timeout(), 0);

  utils::ThreadRunner runner([&r]() {
    r.run();
  }, [&r]() {
    r.shutdown();
  });

  std::this_thread::sleep_for(std::chrono::milliseconds (100));

  UNIT_ASSERT_TRUE(utils::wait_until_running(r));

  std::this_thread::sleep_for(std::chrono::seconds (3));

  r.cancel_timer(echo_conn);

  UNIT_ASSERT_EQUAL(0, echo_conn->interval());
  UNIT_ASSERT_EQUAL(0, echo_conn->next_timeout());

  r.shutdown();

  UNIT_ASSERT_TRUE(echo_conn->timeout_called());
}
