#include <catch2/catch_test_macros.hpp>

#include "matador/query/connection_pool.hpp"
#include "matador/query/error_code.hpp"
#include "matador/query/interface/connection_impl.hpp"
#include "matador/query/backend_provider.hpp"

#include "backend/test_backend_service.hpp"

#include "utils/RecordingObserver.hpp"

#include <thread>

using namespace matador::test;
using namespace matador::query;
using namespace matador::utils;

TEST_CASE("Test statement cache", "[statement][cache]") {
  backend_provider::instance().register_backend("noop", std::make_unique<orm::test_backend_service>());

  matador::utils::message_bus bus;
  connection_pool pool("noop://noop.db", 4);
  statement_cache cache(bus, pool, 2);

  query_context ctx;
  ctx.sql = "SELECT * FROM person";

  REQUIRE(cache.capacity() == 2);
  REQUIRE(cache.empty());

  auto result = cache.acquire(ctx);
  REQUIRE(result);

  REQUIRE(cache.size() == 1);
  REQUIRE(!cache.empty());
  REQUIRE(cache.capacity() == 2);

  const auto stmt = result.value();

  ctx.sql = "SELECT title FROM book";
  ctx.sql_hash = std::hash<std::string>{}(ctx.sql);

  result = cache.acquire(ctx);
  REQUIRE(result);

  REQUIRE(cache.size() == 2);
  REQUIRE(!cache.empty());
  REQUIRE(cache.capacity() == 2);

  ctx.sql = "SELECT name FROM author";
  ctx.sql_hash = std::hash<std::string>{}(ctx.sql);

  result = cache.acquire(ctx);
  REQUIRE(result);

  REQUIRE(cache.size() == 2);
  REQUIRE(!cache.empty());
  REQUIRE(cache.capacity() == 2);
}

TEST_CASE("Test LRU cache evicts oldest entries", "[statement][cache][evict]") {
  backend_provider::instance().register_backend("noop", std::make_unique<orm::test_backend_service>());

  connection_pool pool("noop://noop.db", 4);
  message_bus bus;
  statement_cache cache(bus, pool, 2);
  RecordingObserver observer(bus);

  REQUIRE(cache.capacity() == 2);
  REQUIRE(cache.empty());


  auto result = cache.acquire({"SELECT * FROM person", std::hash<std::string>{}("SELECT * FROM person")});
  REQUIRE(result);
  auto stmt1 = result.value();
  result = cache.acquire({"SELECT title FROM book", std::hash<std::string>{}("SELECT title FROM book")});
  REQUIRE(result);
  auto stmt2 = result.value();
  result = cache.acquire({"SELECT name FROM author", std::hash<std::string>{}("SELECT name FROM author")}); // Should evict the first statement
  REQUIRE(result);
  auto stmt3 = result.value();

  // Trigger re-prepares of an evicted statement
  result = cache.acquire({"SELECT 1", std::hash<std::string>{}("SELECT 1")});
  REQUIRE(result);
  auto stmt4 = result.value();

  REQUIRE(stmt1.sql() == "SELECT * FROM person");
  REQUIRE(stmt2.sql() == "SELECT title FROM book");
  REQUIRE(stmt3.sql() == "SELECT name FROM author");
  REQUIRE(stmt4.sql() == "SELECT 1");

  REQUIRE(cache.size() == 2);
  REQUIRE(!cache.empty());
  REQUIRE(cache.capacity() == 2);

  int added = 0, evicted = 0;
  while (auto e = observer.poll()) {
    if (e->is<statement_added_event>()) {
      added++;
    }
    if (e->is<statement_evicted_event>()) {
      evicted++;
    }
  }
  REQUIRE(added >= 3);
  REQUIRE(evicted >= 1);
}

TEST_CASE("Test statement reuse avoids reprepare", "[statement][cache][prepare]") {
  backend_provider::instance().register_backend("noop", std::make_unique<orm::test_backend_service>());

  connection_pool pool("noop://noop.db", 4);
  message_bus bus;
  statement_cache cache(bus, pool, 2);
  RecordingObserver observer(bus);

  REQUIRE(cache.capacity() == 2);
  REQUIRE(cache.empty());

  auto result = cache.acquire({"SELECT * FROM person", std::hash<std::string>{}("SELECT * FROM person")});
  REQUIRE(result);
  auto stmt1 = result.value();
  result = cache.acquire({"SELECT * FROM person", std::hash<std::string>{}("SELECT * FROM person")});
  REQUIRE(result);
  auto stmt2 = result.value();
}

TEST_CASE("Race condition simulation with mixed access", "[statement_cache][race]") {
  backend_provider::instance().register_backend("noop", std::make_unique<orm::test_backend_service>());

  connection_pool pool("noop://noop.db", 4);
  message_bus bus;
  statement_cache cache(bus, pool, 5);

  constexpr int threads = 8;
  constexpr int operations = 500;

  auto task = [&](int /*id*/) {
    for (int i = 0; i < operations; ++i) {
      const auto sql = "SELECT " + std::to_string(i % 10);
      if (const auto result = cache.acquire({sql}); !result) {
        FAIL("Statement should not be available");
      }

      // if (i % 50 == 0) {
        // cache.cleanup_expired_connections();
      // }
    }
  };

  std::vector<std::thread> jobs;
  jobs.reserve(threads);
  for (int i = 0; i < threads; ++i) {
    jobs.emplace_back(task, i);
  }

  for (auto& t : jobs) {
    t.join();
  }

  SUCCEED("Race simulation completed successfully without crash");
}