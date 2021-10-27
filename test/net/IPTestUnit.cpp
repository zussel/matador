#include "IPTestUnit.hpp"

#include "matador/net/ip.hpp"

using namespace matador;

IPTestUnit::IPTestUnit()
  : matador::unit_test("ip", "ip test unit")
{
  add_test("tcp", [this] { test_tcp(); }, "tcp test");
  add_test("udp", [this] { test_udp(); }, "udp test");
}

void IPTestUnit::test_tcp()
{
  tcp tcp_v4 = tcp::v4();

  UNIT_ASSERT_EQUAL(SOCK_STREAM, tcp_v4.type());
  UNIT_ASSERT_EQUAL(IPPROTO_TCP, tcp_v4.protocol());
  UNIT_ASSERT_EQUAL(PF_INET, tcp_v4.family());

  tcp tcp_v6 = tcp::v6();

  UNIT_ASSERT_EQUAL(SOCK_STREAM, tcp_v6.type());
  UNIT_ASSERT_EQUAL(IPPROTO_TCP, tcp_v6.protocol());
  UNIT_ASSERT_EQUAL(PF_INET6, tcp_v6.family());
}

void IPTestUnit::test_udp()
{
  udp udp_v4 = udp::v4();

  UNIT_ASSERT_EQUAL(SOCK_DGRAM, udp_v4.type());
  UNIT_ASSERT_EQUAL(IPPROTO_UDP, udp_v4.protocol());
  UNIT_ASSERT_EQUAL(PF_INET, udp_v4.family());

  udp udp_v6 = udp::v6();

  UNIT_ASSERT_EQUAL(SOCK_DGRAM, udp_v6.type());
  UNIT_ASSERT_EQUAL(IPPROTO_UDP, udp_v6.protocol());
  UNIT_ASSERT_EQUAL(PF_INET6, udp_v6.family());
}
