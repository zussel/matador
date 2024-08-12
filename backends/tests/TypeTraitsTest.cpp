#include <catch2/catch_test_macros.hpp>

#include "ColorEnumTraits.hpp"

#include "matador/sql/connection.hpp"
#include "matador/sql/column_generator.hpp"

#include "connection.hpp"

#include "models/location.hpp"


using namespace matador::sql;
using namespace matador::test;

class TypeTraitsTestFixture
{
public:
  TypeTraitsTestFixture()
  : db(matador::test::connection::dns)
  , schema(db.dialect().default_schema_name())
  {
    db.open();
    db.query(schema).create()
    .table<location>("location")
    .execute();
  }

  ~TypeTraitsTestFixture()
  {
    db.query(schema).drop().table("location").execute();
  }

protected:
  matador::sql::connection db;
  matador::sql::schema schema;

private:
  void drop_table_if_exists(const std::string &table_name) {
    if (db.exists(table_name)) {
      db.query(schema).drop().table(table_name).execute();
    }
  }
};

TEST_CASE_METHOD(TypeTraitsTestFixture, "Special handling of attributes with type traits", "[typetraits]")
{
  schema.attach<location>("location");
  SECTION("Insert and select with direct execution") {
    location loc{1, "center", {1, 2, 3}, Color::Black};

    auto res = db
      .query(schema)
      .insert()
      .into("location", column_generator::generate<location>(schema, true))
      .values(loc)
      .execute();
    REQUIRE(res == 1);

    auto result = db
      .query(schema)
      .select(column_generator::generate<location>(schema, true))
      .from("location")
      .fetch_all<location>();

    for (const auto &l: result) {
      REQUIRE(l.name == "center");
    }
  }

  SECTION("Insert and select with prepared statement") {
    location loc{1, "center", {1, 2, 3}, Color::Black};

    auto stmt = db
      .query(schema)
      .insert()
      .into("location", column_generator::generate<location>(schema, true))
      .values<location>()
      .prepare();

    auto res = stmt
      .bind(loc)
      .execute();
    REQUIRE(res == 1);

    auto result = db
      .query(schema)
      .select(column_generator::generate<location>(schema, true))
      .from("location")
      .fetch_all<location>();

    for (const auto &l: result) {
      REQUIRE(l.name == "center");
    }
  }
}