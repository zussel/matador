#include "SocketTest.hpp"

#include "matador/net/ip.hpp"

using namespace matador;

SocketTest::SocketTest()
  : matador::unit_test("socket", "socket test unit")
{
  add_test("socket_v4", [this] { test_socket_v4(); }, "socket v4 test");
  add_test("acceptor_v4", [this] { test_acceptor_v4(); }, "acceptor v4 test");
}

void SocketTest::test_socket_v4()
{
  tcp::socket ts1(tcp::v4());

  UNIT_ASSERT_FALSE(ts1.non_blocking());
  UNIT_ASSERT_TRUE(ts1.is_open());
  UNIT_ASSERT_TRUE(ts1.id() > 0);

  tcp::socket ts2;
  UNIT_ASSERT_FALSE(ts2.is_open());
  ts2.open(tcp::v4());
  UNIT_ASSERT_TRUE(ts2.is_open());
  UNIT_ASSERT_FALSE(ts2.non_blocking());
  ts2.non_blocking(true);
  UNIT_ASSERT_TRUE(ts2.non_blocking());

  int fd = ts1.id();

  int released_fd = ts1.release();

  UNIT_ASSERT_EQUAL(fd, released_fd);
  UNIT_ASSERT_EQUAL(0, ts1.id());
}

void SocketTest::test_acceptor_v4()
{
  tcp::acceptor acceptor;

  UNIT_ASSERT_FALSE(acceptor.is_open());
  UNIT_ASSERT_FALSE(acceptor.reuse_address());

  tcp::peer local(address::v4::any(), 12345);
  UNIT_ASSERT_EQUAL(0, acceptor.bind(local));

  UNIT_ASSERT_TRUE(acceptor.reuse_address());
  UNIT_ASSERT_TRUE(acceptor.is_open());
  UNIT_ASSERT_TRUE(acceptor.id() > 0);

  UNIT_ASSERT_EQUAL(0, acceptor.listen(5));

  tcp::peer localhost12345(address::v4::loopback(), 12345);
  tcp::socket s(tcp::v4());
  UNIT_ASSERT_TRUE(s.connect(localhost12345));

  tcp::socket remote;
  UNIT_ASSERT_TRUE(acceptor.accept(remote) > 0);
  UNIT_ASSERT_TRUE(remote.id() > 0);
}
