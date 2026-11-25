#include <catch2/catch_test_macros.hpp>

#include "fixtures/SchemaFixture.hpp"

#include "matador/query/runtime/backend_provider.hpp"
#include "matador/query/runtime/connection_pool.hpp"
#include "matador/query/runtime/schema_manager.hpp"

#include "matador/query/schema/schema.hpp"

#include "models/department.hpp"
#include "models/recipe.hpp"

using namespace matador;
using namespace matador::test;

TEST_CASE_METHOD(SchemaFixture, "Test schema one-two-many", "[schema][one-to-many]") {
  using namespace matador::test;
  query::schema repo;
  query::schema_manager manager(repo);

  auto result = repo.attach<department>("departments")
    .and_then([&repo] { return repo.attach<employee>("employees"); });
  REQUIRE(result);

  auto conn = pool.acquire();
  result = manager.create(*conn);
  REQUIRE(result);

  auto exists_result = manager.table_exists("departments", *conn);
  REQUIRE(exists_result.is_ok());
  REQUIRE(exists_result.value());

  result = manager.drop(*conn);
  REQUIRE(result);

  exists_result = manager.table_exists("departments", *conn);
  REQUIRE(exists_result.is_ok());
  REQUIRE(!exists_result.value());
}

TEST_CASE_METHOD(SchemaFixture, "Test schema many-to-many", "[schema][many-to-many]") {
  using namespace matador::test;
  query::schema repo;
  query::schema_manager manager(repo);

  auto result = repo.attach<recipe>("recipes")
    .and_then([&repo] { return repo.attach<ingredient>("ingredients"); });
  REQUIRE(result);

  auto conn = pool.acquire();
  result = manager.create(*conn);
  REQUIRE(result);

  auto exists_result = manager.table_exists("recipes", *conn);
  REQUIRE(exists_result.is_ok());
  REQUIRE(exists_result.value());

  result = manager.drop(*conn);
  REQUIRE(result);

  exists_result = manager.table_exists("recipes", *conn);
  REQUIRE(exists_result.is_ok());
  REQUIRE(!exists_result.value());
}
