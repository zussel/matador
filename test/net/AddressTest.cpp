#include "AddressTest.hpp"

#include "matador/net/address.hpp"
#include "matador/net/ip.hpp"

AddressTest::AddressTest()
  : matador::unit_test("address", "ip address test unit")
{
  add_test("address_v4", std::bind(&AddressTest::test_address_v4, this), "ip address v4 test");
  add_test("address_v6", std::bind(&AddressTest::test_address_v6, this), "ip address v6 test");
  add_test("peer_v4", std::bind(&AddressTest::test_peer_v4, this), "ip peer v4 test");
  add_test("peer_v6", std::bind(&AddressTest::test_peer_v6, this), "ip peer v6 test");
  add_test("ip", std::bind(&AddressTest::test_ip, this), "ip test");
}

using namespace matador;

void AddressTest::test_address_v4()
{
  address google = address::v4::from_hostname("www.google.org");

  unsigned long ulong_google_address(455143384);
  unsigned long ulong_loopback_address(16777343);
  unsigned long ulong_any_address(0);
  unsigned long ulong_broadcast_address(4294967295);
  unsigned long ulong_denic_address(212491089);

  UNIT_ASSERT_EQUAL("216.239.32.27", google.to_string());
  UNIT_ASSERT_EQUAL(ulong_google_address, google.to_ulong());

  address localhost = address::v4::from_hostname("localhost");
  address lh127 = address::v4::from_ip("127.0.0.1");

  UNIT_ASSERT_EQUAL(localhost.to_string(), lh127.to_string());
  UNIT_ASSERT_EQUAL(localhost.to_ulong(), lh127.to_ulong());

  address loopback = address::v4::loopback();

  UNIT_ASSERT_EQUAL("127.0.0.1", loopback.to_string());
  UNIT_ASSERT_EQUAL(ulong_loopback_address, loopback.to_ulong());

  address broadcast = address::v4::broadcast();

  UNIT_ASSERT_EQUAL("255.255.255.255", broadcast.to_string());
  UNIT_ASSERT_EQUAL(ulong_broadcast_address, broadcast.to_ulong());

  address any = address::v4::any();

  UNIT_ASSERT_EQUAL("0.0.0.0", any.to_string());
  UNIT_ASSERT_EQUAL(ulong_any_address, any.to_ulong());

  address denic = address::v4::from_hostname("www.denic.de");

  UNIT_ASSERT_EQUAL("81.91.170.12", denic.to_string());
  UNIT_ASSERT_EQUAL(ulong_denic_address, denic.to_ulong());

  localhost = address::v4::from_hostname(std::string("localhost"));
  lh127 = address::v4::from_ip(std::string("127.0.0.1"));

  UNIT_ASSERT_EQUAL(localhost.to_ulong(), lh127.to_ulong());
  UNIT_ASSERT_FALSE(localhost.is_v6());
  UNIT_ASSERT_NOT_NULL(localhost.addr_v4());

  address assigned_address;
  assigned_address = lh127;
  UNIT_ASSERT_EQUAL(assigned_address.to_ulong(), lh127.to_ulong());

  auto nulladdr = address::v4::from_hostname(nullptr);
  UNIT_ASSERT_EQUAL(0UL, nulladdr.size());

  nulladdr = address::v4::from_ip(nullptr);
  UNIT_ASSERT_EQUAL(0UL, nulladdr.size());
}

void AddressTest::test_address_v6()
{
  address lh127 = address::v6::from_ip("::1");

  auto str_127 = lh127.to_string();

  UNIT_ASSERT_EQUAL("::1", lh127.to_string());

  address loopback = address::v6::loopback();

  UNIT_ASSERT_EQUAL("::1", loopback.to_string());

  address multicast = address::v6::broadcast();

  auto str_mc = multicast.to_string();

  UNIT_ASSERT_EQUAL("ff02::1", str_mc);

  address any = address::v6::any();
  auto str_any = any.to_string();

  UNIT_ASSERT_EQUAL("::", str_any);
  UNIT_ASSERT_NOT_NULL(lh127.addr_v6());
}

void AddressTest::test_peer_v4()
{
  address localhost = address::v4::loopback();
  tcp::peer localhost8080(localhost, 8080);
  size_t peer_size(16);

  UNIT_ASSERT_EQUAL(8080, localhost8080.port());
  UNIT_ASSERT_EQUAL(tcp::v4().family(), localhost8080.protocol().family());
  UNIT_ASSERT_EQUAL(tcp::v4().protocol(), localhost8080.protocol().protocol());
  UNIT_ASSERT_EQUAL(tcp::v4().type(), localhost8080.protocol().type());
  UNIT_ASSERT_EQUAL(peer_size, localhost8080.size());
  UNIT_ASSERT_EQUAL("127.0.0.1:8080", localhost8080.to_string());
}

void AddressTest::test_peer_v6()
{
  address localhost = address::v6::loopback();
  tcp::peer localhost8080(localhost, 8080);
  size_t peer_size(28);

  UNIT_ASSERT_EQUAL(8080, localhost8080.port());
  UNIT_ASSERT_EQUAL(tcp::v6().family(), localhost8080.protocol().family());
  UNIT_ASSERT_EQUAL(tcp::v6().protocol(), localhost8080.protocol().protocol());
  UNIT_ASSERT_EQUAL(tcp::v6().type(), localhost8080.protocol().type());
  UNIT_ASSERT_EQUAL(peer_size, localhost8080.size());
  UNIT_ASSERT_EQUAL("::1:8080", localhost8080.to_string());
}

void AddressTest::test_ip()
{
  auto *addr = new sockaddr_in;
  auto ret = os::inet_pton(AF_INET, "127.0.0.1", &addr->sin_addr);
  UNIT_ASSERT_EQUAL(1, ret);

  ret = os::inet_pton(AF_INET, "127.0.0.1.0", &addr->sin_addr);
  UNIT_ASSERT_EQUAL(0, ret);

  ret = os::inet_pton(90, "192.168.178.13", &addr->sin_addr);
  UNIT_ASSERT_EQUAL(-1, ret);

  delete addr;
}
