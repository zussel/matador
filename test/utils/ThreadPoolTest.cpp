#include "ThreadPoolTest.hpp"

#include "matador/utils/thread_pool.hpp"

using namespace matador;

ThreadPoolTest::ThreadPoolTest()
  : unit_test("thread_pool", "thread pool test")
{
  add_test("thread_pool", [this] { test_threadpool(); }, "thread pool test");
}

void task(int )
{
//  std::cout << "thread: starting task " << num << "\n";
  std::this_thread::sleep_for(std::chrono::seconds (1));
//  std::cout << "thread: finished task " << num << "\n";
}

void ThreadPoolTest::test_threadpool()
{
  thread_pool tp(2);

  for (int i = 0; i < 8; ++i) {
//    std::cout << "schedule task " << i+1 << "\n";
    tp.schedule([num = i + 1] { return task(num); });
  }

  UNIT_ASSERT_EQUAL(2UL, tp.size());

  std::this_thread::sleep_for(std::chrono::seconds (2));

  tp.wait();
  tp.shutdown();

  UNIT_ASSERT_EQUAL(0UL, tp.pending());
}
