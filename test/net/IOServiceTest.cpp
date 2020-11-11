#include "IOServiceTest.hpp"
#include "IOEchoServer.hpp"
#include "NetUtils.hpp"

#include <thread>

#ifndef _WIN32
#include <unistd.h>
#endif

using namespace matador;
using namespace ::detail;

//class IOServiceThreadWrapper
//{
//public:
//  IOServiceThreadWrapper(unsigned short port)
//    : service_(port)
//  {}
//
//  ~IOServiceThreadWrapper()
//  {
//    if (service_.is_running()) {
//      stop();
//    }
//    if (reactor_thread_.joinable()) {
//      reactor_thread_.join();
//    }
//  }
//
//  void add(const std::shared_ptr<acceptor> &ac)
//  {
//    service_.register_handler(ac, event_type::ACCEPT_MASK);
//  }
//
//  void start()
//  {
//    reactor_thread_ = std::thread([this] {
//      service_.run();
//      // sleep for some seconds to ensure valid thread join
//      std::this_thread::sleep_for(std::chrono::seconds (2));
//    });
//  }
//  void stop()
//  {
//    service_.shutdown();
//  }
//
//  io_service& get()
//  {
//    return service_;
//  }
//
//private:
//  std::thread reactor_thread_;
//  io_service service_;
//};

IOServiceTest::IOServiceTest()
  : matador::unit_test("io_service", "io service test unit")
{
//  add_test("shutdown", std::bind(&IOServiceTest::test_shutdown, this), "io service shutdown test");
//  add_test("send_receive", std::bind(&IOServiceTest::test_send_receive, this), "io service send and receive test");
}

void IOServiceTest::test_shutdown()
{
  IOEchoServer server(7779);

  std::thread([&server] {
    server.run();
    // sleep for some seconds to ensure valid thread join
    std::this_thread::sleep_for(std::chrono::seconds (2));
  });

  UNIT_ASSERT_TRUE(utils::wait_until_running(server.service()));

  server.service().shutdown();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(server.service()));
}

void IOServiceTest::test_send_receive()
{
  IOEchoServer server(7780);

  std::thread([&server] {
    server.run();
    // sleep for some seconds to ensure valid thread join
    std::this_thread::sleep_for(std::chrono::seconds (2));
  });

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

  server.service().shutdown();

  UNIT_ASSERT_TRUE(utils::wait_until_stopped(server.service()));
}
