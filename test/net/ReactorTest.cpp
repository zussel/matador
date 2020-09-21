#include <thread>
#include <matador/net/reactor.hpp>
#include "ReactorTest.hpp"

using namespace matador;

ReactorTest::ReactorTest()
  : matador::unit_test("reactor", "reactor test unit")
{
  add_test("timer", std::bind(&ReactorTest::test_reactor_timer, this), "recator timer test");

}

void ReactorTest::test_reactor_timer()
{
  reactor r;

  std::thread server_thread([&r] {
    r.run();
  });

  server_thread.join();
}
