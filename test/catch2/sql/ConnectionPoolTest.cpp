#include <catch2/catch_test_macros.hpp>

#include "matador/sql/connection_pool.hpp"
#include "matador/sql/noop_connection.hpp"

#include "tools/auto_reset_event.hpp"

using namespace matador::sql;
using namespace matador::test::utils;

TEST_CASE("Create connection pool", "[connection pool]") {
  using pool_t = connection_pool<noop_connection>;

  pool_t pool("noop://noop.db", 4);

  REQUIRE(pool.size() == 4);
  REQUIRE(pool.idle() == 4);
  REQUIRE(pool.inuse() == 0);

  auto ptr = pool.acquire();
  REQUIRE(ptr.valid());
  REQUIRE(ptr.id().value() > 0);
  REQUIRE(ptr->is_open());

  REQUIRE(pool.idle() == 3);
  REQUIRE(pool.inuse() == 1);

  pool.release(ptr);
  REQUIRE(!ptr.valid());
  REQUIRE(pool.idle() == 4);
  REQUIRE(pool.inuse() == 0);

  ptr = pool.acquire(3);
  REQUIRE(ptr.valid());
  REQUIRE(ptr.id() == 3);
  REQUIRE(ptr->is_open());
  {
    auto ptr2 = pool.acquire();
    REQUIRE(ptr2.valid());
    REQUIRE(ptr2->is_open());

    REQUIRE(pool.idle() == 2);
    REQUIRE(pool.inuse() == 2);
  }

  REQUIRE(pool.idle() == 3);
  REQUIRE(pool.inuse() == 1);

  pool.release(ptr);
  REQUIRE(!ptr.valid());
  REQUIRE(pool.idle() == 4);
  REQUIRE(pool.inuse() == 0);
}

TEST_CASE("Acquire connection by id", "[connection pool]") {
  using pool_t = connection_pool<noop_connection>;

  pool_t pool("noop://noop.db", 4);

  REQUIRE(pool.size() == 4);
  REQUIRE(pool.idle() == 4);
  REQUIRE(pool.inuse() == 0);

  auto ptr = pool.acquire();
  REQUIRE(ptr.valid());
  REQUIRE(ptr.id());
  REQUIRE(ptr.id().value() > 0);
  REQUIRE(ptr->is_open());

  auto same_ptr = pool.acquire(ptr.id().value());

  REQUIRE(!same_ptr.valid());

  const auto connection_id = ptr.id().value();

  pool.release(ptr);
  REQUIRE(!ptr.valid());

  same_ptr = pool.acquire(connection_id);

  REQUIRE(same_ptr.valid());
  REQUIRE(same_ptr.id() == connection_id);
}

TEST_CASE("Try acquire connection", "[connection pool][try acquire]") {
  using pool_t = connection_pool<noop_connection>;

  pool_t pool("noop://noop.db", 1);

  REQUIRE(pool.size() == 1);
  REQUIRE(pool.idle() == 1);
  REQUIRE(pool.inuse() == 0);

  auto ptr = pool.try_acquire();
  REQUIRE(ptr.valid());
  REQUIRE(ptr.id());
  REQUIRE(ptr.id().value() > 0);
  REQUIRE(ptr->is_open());
  REQUIRE(pool.size() == 1);
  REQUIRE(pool.idle() == 0);
  REQUIRE(pool.inuse() == 1);

  auto ptr2 = pool.try_acquire();
  REQUIRE(!ptr2.valid());

  pool.release(ptr);
  REQUIRE(!ptr.valid());
  REQUIRE(pool.size() == 1);
  REQUIRE(pool.idle() == 1);
  REQUIRE(pool.inuse() == 0);

  ptr2 = pool.try_acquire();
  REQUIRE(ptr2.valid());
  REQUIRE(ptr2.id());
  REQUIRE(ptr2.id().value() > 0);
  REQUIRE(ptr2->is_open());
  REQUIRE(pool.size() == 1);
  REQUIRE(pool.idle() == 0);
  REQUIRE(pool.inuse() == 1);

  pool.release(ptr2);

  auto_reset_event reset_main_event;
  auto_reset_event reset_thread_event;

  std::thread t([&reset_main_event, &reset_thread_event, &pool]() {
    auto c1 = pool.acquire();
    REQUIRE(c1.valid());
    REQUIRE(c1.id());
    REQUIRE(c1.id().value() > 0);

    reset_main_event.set();

    reset_thread_event.wait_one();

    pool.release(c1);
    REQUIRE(!c1.valid());
    reset_main_event.set();
  });
  reset_main_event.wait_one();

  ptr2 = pool.try_acquire();
  REQUIRE(!ptr2.valid());

  reset_thread_event.set();

  reset_main_event.wait_one();
  ptr2 = pool.try_acquire();
  REQUIRE(ptr2.valid());
  REQUIRE(ptr2.id());
  REQUIRE(ptr2.id().value() > 0);

  t.join();
}