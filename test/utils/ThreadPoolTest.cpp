#include "ThreadPoolTest.hpp"

#include "matador/utils/thread_pool.hpp"

using namespace matador;

ThreadPoolTest::ThreadPoolTest()
  : unit_test("thread_pool", "thread pool test")
{
  add_test("thread_pool", [this] { test_thread_pool(); }, "thread pool test");
}

void task(int )
{
  std::this_thread::sleep_for(std::chrono::seconds (1));
}

void ThreadPoolTest::test_thread_pool()
{
  thread_pool tp(2);

  UNIT_ASSERT_EQUAL(2UL, tp.size());

  for (int i = 0; i < 8; ++i) {
    tp.schedule([num = i + 1] { return task(num); });
  }

  std::this_thread::sleep_for(std::chrono::seconds (2));

  tp.wait();
  tp.shutdown();

  UNIT_ASSERT_EQUAL(0UL, tp.pending());
}
