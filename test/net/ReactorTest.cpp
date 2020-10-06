#include "ReactorTest.hpp"

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

ReactorTest::ReactorTest()
  : matador::unit_test("reactor", "reactor test unit")
{
  add_test("shutdown", std::bind(&ReactorTest::test_shutdown, this), "reactor shutdown test");
  add_test("send_receive", std::bind(&ReactorTest::test_send_receive, this), "reactor send and receive test");
}

bool wait_until_running(reactor &r, int retries = 10)
{
  while (!r.is_running() && retries-- > 0) {
#ifdef _WIN32
    ::Sleep(1000);
#else
    ::usleep(500);
#endif
  }

  return r.is_running();
}

void ReactorTest::test_shutdown()
{
//  auto logsink = matador::create_file_sink("reactor.log");
//  matador::add_log_sink(logsink);
  matador::add_log_sink(matador::create_stdout_sink());

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

//  logsink->close();
//  matador::os::remove("reactor.log");

  log_manager::instance().clear();
}

void log_errno(logger &log, const char *msg, int err)
{
  char error_buffer[1024];
  os::strerror(err, error_buffer, 1024);
  log.error(msg, error_buffer);
}

void ReactorTest::test_send_receive()
{
//  auto logsink = matador::create_file_sink("reactor.log");
//  matador::add_log_sink(logsink);
  matador::add_log_sink(matador::create_stdout_sink());

  logger log(create_logger("Client"));

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

  int ret = client.open(tcp::v4());
  if (ret < 0) {
    log_errno(log, "Error while opening client: %s", errno);
  } else {
    auto srv = tcp::peer(address::v4::loopback(), 7777);
    log.info("client socket open (fd: %d), connecting to %s", client.id(), srv.to_string().c_str());
    ret = client.connect(srv);
    if (ret < 0) {
      log_errno(log, "Error while connecting to server: %s", errno);
      client.close();
    } else {
      log.info("client connection established, sending data");
      buffer data;
      data.append("hallo", 5);
      size_t len = client.send(data);
      UNIT_ASSERT_EQUAL(5UL, len);
      data.clear();
      len = client.receive(data);
      log.info("receveived data (size: %d)", len);
      UNIT_ASSERT_EQUAL(5UL, len);
      client.close();
    }
  }

  r.shutdown();

  server_thread.join();

  ac->close();

//  logsink->close();
//  matador::os::remove("reactor.log");
 
  log_manager::instance().clear();
}
