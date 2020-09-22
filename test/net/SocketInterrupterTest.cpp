#include "SocketInterrupterTest.hpp"

#include "matador/net/socket_interrupter.hpp"

using namespace matador;

SocketInterrupterTest::SocketInterrupterTest()
  : unit_test("socket_interrupter", "socket interrupter test")
{
  add_test("interrupter", std::bind(&SocketInterrupterTest::test_interrupter, this), "socket interrupter test");
}

void SocketInterrupterTest::test_interrupter()
{
  socket_interrupter interrupter;

  UNIT_ASSERT_TRUE(interrupter.socket_id() > 0);

  interrupter.interrupt();

  UNIT_ASSERT_TRUE(interrupter.reset());
}
