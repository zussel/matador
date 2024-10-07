#include <catch2/catch_test_macros.hpp>

#include "ColorEnumTraits.hpp"

#include "matador/sql/connection.hpp"
#include "matador/sql/column_generator.hpp"
#include "matador/sql/query.hpp"

#include "QueryFixture.hpp"

#include "models/location.hpp"

using namespace matador::sql;
using namespace matador::test;

class TypeTraitsTestFixture : public QueryFixture
{
public:
  TypeTraitsTestFixture()
  {
    db.open();
    const auto res = query::create()
      .table<location>("location", schema)
      .execute(db);
    tables_to_drop.emplace("location");
  }
};

TEST_CASE_METHOD(TypeTraitsTestFixture, "Special handling of attributes with type traits", "[typetraits]")
{
  schema.attach<location>("location");
  SECTION("Insert and select with direct execution") {
    location loc{1, "center", {1, 2, 3}, Color::Black};

    auto res = query::insert()
      .into("location", column_generator::generate<location>(schema, true))
      .values(loc)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(*res == 1);

    auto result = query::select(column_generator::generate<location>(schema, true))
      .from("location")
      .fetch_one<location>(db);

    REQUIRE(result != nullptr);
    REQUIRE(result->name == "center");
    REQUIRE(result->color == Color::Black);
    REQUIRE(result->coord.x == 1);
    REQUIRE(result->coord.y == 2);
    REQUIRE(result->coord.z == 3);
  }

  SECTION("Insert and select with prepared statement") {
    location loc{1, "center", {1, 2, 3}, Color::Black};

    auto stmt = query::insert()
      .into("location", column_generator::generate<location>(schema, true))
      .values<location>()
      .prepare(db);

    auto res = stmt
      .bind(loc)
      .execute();
    REQUIRE(res.is_ok());
    REQUIRE(*res == 1);

    auto result = query::select(column_generator::generate<location>(schema, true))
      .from("location")
      .fetch_one<location>(db);

    REQUIRE(result != nullptr);
    REQUIRE(result->name == "center");
    REQUIRE(result->color == Color::Black);
    REQUIRE(result->coord.x == 1);
    REQUIRE(result->coord.y == 2);
    REQUIRE(result->coord.z == 3);
  }
}