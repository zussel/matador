#include "AddressResolverTest.hpp"

#include "matador/net/ip.hpp"

AddressResolverTest::AddressResolverTest()
  : matador::unit_test("address_resolver", "ip address resolver test unit")
{
  add_test("resolver_v4", [this] { test_resolver_v4(); }, "ip address resolver v4 test");

}

void AddressResolverTest::test_resolver_v4()
{

  matador::tcp::resolver resolver;

  auto peers = resolver.resolve(std::string("localhost"), std::string("80"));

  UNIT_ASSERT_FALSE(peers.empty());
  UNIT_ASSERT_GREATER(peers.size(), 0UL);

//  for (const auto &p : peers) {
//    if (p.addr().is_v4()) {
//      UNIT_ASSERT_EQUAL("127.0.0.1:80", p.to_string());
//    } else {
//      UNIT_ASSERT_EQUAL("::1:80", p.to_string());
//    }
//  }
}
