#include "ReactorTest.hpp"
#include "NetUtils.hpp"

#include "matador/net/reactor.hpp"
#include "matador/net/acceptor.hpp"
#include "matador/net/connector.hpp"

#include "EchoServer.hpp"

#include <thread>
#include <utility>

#ifndef _WIN32
#include <unistd.h>

#endif

using namespace matador;
using namespace ::detail;

class ReactorThreadWrapper
{
public:
  ReactorThreadWrapper() = default;
  ~ReactorThreadWrapper()
  {
    if (reactor_.is_running()) {
      stop();
    }
    if (reactor_thread_.joinable()) {
      reactor_thread_.join();
    }
  }

  void add(const std::shared_ptr<acceptor> &ac)
  {
    reactor_.register_handler(ac, event_type::ACCEPT_MASK);
  }

  void add(const std::shared_ptr<connector> &co, unsigned short port)
  {
    tcp::resolver resolver;
    auto endpoints = resolver.resolve("localhost", std::to_string(port));
    co->connect(reactor_, endpoints);
  }

  void schedule(const std::shared_ptr<handler> &hndlr, time_t offset, time_t interval)
  {
    reactor_.schedule_timer(hndlr, offset, interval);
  }

  void cancel(const std::shared_ptr<handler> &hndlr)
  {
    reactor_.cancel_timer(hndlr);
  }

  void start()
  {
    reactor_thread_ = std::thread([this] {
      reactor_.run();
      // sleep for some seconds to ensure valid thread join
      std::this_thread::sleep_for(std::chrono::seconds (2));
    });
  }
  void stop()
  {
    reactor_.shutdown();
  }

  reactor& get()
  {
    return reactor_;
  }

private:
  std::thread reactor_thread_;
  reactor reactor_;
};

ReactorTest::ReactorTest()
  : matador::unit_test("reactor", "reactor test unit")
{
  add_test("event_types", std::bind(&ReactorTest::test_event_types, this), "event types test");
  add_test("fdsets", std::bind(&ReactorTest::test_fdset, this), "reactor fdsets test");
  add_test("shutdown", std::bind(&ReactorTest::test_shutdown, this), "reactor shutdown test");
  add_test("send_receive", std::bind(&ReactorTest::test_send_receive, this), "reactor send and receive test");
//  add_test("connector", std::bind(&ReactorTest::test_connector, this), "reactor connector test");
  add_test("timeout", std::bind(&ReactorTest::test_timeout, this), "reactor schedule timeout test");

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
  auto ep = tcp::peer(address::v4::any(), 7777);
  auto ac = std::make_shared<acceptor>(ep, [](tcp::socket sock, tcp::peer p, acceptor *) {
    auto cl = std::make_shared<EchoServer>();
    cl->init(std::move(sock), std::move(p));
    return cl;
  });

  ReactorThreadWrapper wrapper;

  wrapper.add(ac);

  wrapper.start();

  UNIT_ASSERT_TRUE(utils::wait_until_running(wrapper.get()));

  wrapper.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(wrapper.get()));
}

void ReactorTest::test_send_receive()
{
  auto echo_conn = std::make_shared<EchoServer>();

  auto ep = tcp::peer(address::v4::any(), 7778);
  auto ac = std::make_shared<acceptor>(ep, [echo_conn](tcp::socket sock, tcp::peer p, acceptor *) {
    echo_conn->init(std::move(sock), std::move(p));
    return echo_conn;
  });

  ReactorThreadWrapper wrapper;

  wrapper.add(ac);

  wrapper.start();

  UNIT_ASSERT_TRUE(utils::wait_until_running(wrapper.get()));

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

  wrapper.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(wrapper.get()));
}

void ReactorTest::test_connector()
{
  // setup acceptor
  tcp::acceptor acceptor;

  UNIT_ASSERT_FALSE(acceptor.is_open());
  UNIT_ASSERT_EQUAL(0, acceptor.reuse_address());

  tcp::peer local(address::v4::any(), 12345);
  UNIT_ASSERT_EQUAL(0, acceptor.bind(local));

  UNIT_ASSERT_EQUAL(1, acceptor.reuse_address());
  UNIT_ASSERT_TRUE(acceptor.is_open());
  UNIT_ASSERT_TRUE(acceptor.id() > 0);

  UNIT_ASSERT_EQUAL(0, acceptor.listen(5));

  // setup connector in reactor wrapper
  auto echo_conn = std::make_shared<EchoServer>();

  auto echo_connector = std::make_shared<connector>([echo_conn](const tcp::socket& sock, const tcp::peer &p, connector *cnnctr) {
    echo_conn->init(sock, p);
    return echo_conn;
  });

  ReactorThreadWrapper wrapper;
  wrapper.add(echo_connector, 7890);

  wrapper.start();

  // accept connection
  tcp::socket remote;
  UNIT_ASSERT_TRUE(acceptor.accept(remote) > 0);
  UNIT_ASSERT_TRUE(remote.id() > 0);

//  remote.send()

}

void ReactorTest::test_timeout()
{
  auto echo_conn = std::make_shared<EchoServer>();

  ReactorThreadWrapper wrapper;

  wrapper.schedule(echo_conn, 1, 1);

  UNIT_ASSERT_EQUAL(1, echo_conn->interval());
  UNIT_ASSERT_GREATER(echo_conn->next_timeout(), 0);

  wrapper.start();

  UNIT_ASSERT_TRUE(utils::wait_until_running(wrapper.get()));

  std::this_thread::sleep_for(std::chrono::seconds (2));

  wrapper.cancel(echo_conn);

  UNIT_ASSERT_EQUAL(0, echo_conn->interval());
  UNIT_ASSERT_EQUAL(0, echo_conn->next_timeout());

  wrapper.stop();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(wrapper.get()));
  UNIT_ASSERT_TRUE(echo_conn->timeout_called());
}
