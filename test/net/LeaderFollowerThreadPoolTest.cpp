#include "LeaderFollowerThreadPoolTest.hpp"

#include "matador/net/leader_follower_thread_pool.hpp"

using namespace matador;

LeaderFollowerThreadPoolTest::LeaderFollowerThreadPoolTest()
: unit_test("lf_thread_pool", "leader follower thread pool test")
{
  add_test("thread_pool", [this] { test_threadpool(); }, "leader follower thread pool test");
}

struct runner
{
  leader_follower_thread_pool lftp_;

  runner() : lftp_(2, [this]() { handle_task(); }) {}

  void handle_task() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lftp_.promote_new_leader();
    ++counter;
  }

  std::atomic_uint counter {0};
};

void LeaderFollowerThreadPoolTest::test_threadpool()
{
  runner r;

  UNIT_ASSERT_EQUAL(0UL, r.lftp_.size());
  UNIT_ASSERT_EQUAL(2UL, r.lftp_.num_follower());
  UNIT_ASSERT_EQUAL(std::thread::id {}, r.lftp_.leader());

  UNIT_ASSERT_FALSE(r.lftp_.is_running());

  r.lftp_.start();

  UNIT_ASSERT_EQUAL(2UL, r.lftp_.size());
  UNIT_ASSERT_TRUE(r.lftp_.is_running());

  std::this_thread::sleep_for(std::chrono::milliseconds (1600));

  r.lftp_.shutdown();

  UNIT_ASSERT_EQUAL(4U, r.counter);
  UNIT_ASSERT_FALSE(r.lftp_.is_running());
}
