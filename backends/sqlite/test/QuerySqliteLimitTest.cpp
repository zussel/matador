#include <catch2/catch_test_macros.hpp>

#include "../../tests/QueryFixture.hpp"

#include "matador/sql/condition.hpp"

using namespace matador::test;
using namespace matador::sql;

TEST_CASE_METHOD(QueryFixture, "Test select with limit", "[query][select][limit]") {
  const auto q = db.query(schema)
    .select({"id"})
    .from("person")
    .where("id"_col > 1)
    .order_by("id").asc()
    .limit(5)
    .build();

  REQUIRE(q.table.name == "person");
  REQUIRE(q.sql == R"(SELECT "id" FROM "main"."person" WHERE "id" > 1 ORDER BY "id" ASC LIMIT 5)");
}

TEST_CASE_METHOD(QueryFixture, "Test update with limit", "[query][update][limit]") {
  const auto q = db.query(schema)
    .update({"person"})
    .set({{"name", "george"}})
    .where("id"_col > 1)
    .limit(5)
    .build();

  REQUIRE(q.table.name == "person");
  REQUIRE(q.sql == R"(UPDATE "main"."person" SET "name"='george' WHERE "rowid" IN (SELECT "rowid" FROM "person" WHERE "id" > 1 LIMIT 5))");
}