#include <catch2/catch_test_macros.hpp>

#include "matador/sql/column_definition.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/connection.hpp"

#include "matador/object/object_ptr.hpp"

#include "connection.hpp"

#include "models/airplane.hpp"

using namespace matador::sql;
using namespace matador::test;

namespace matador::test::detail {
template<class Type, typename... Args>
[[maybe_unused]] object_ptr<Type> make_object_ptr(Args&&... args)
{
  return object_ptr(new Type(std::forward<Args>(args)...));
}
}

class StatementTestFixture
{
public:
  StatementTestFixture()
  : db(matador::test::connection::dns)
  , schema(db.dialect().default_schema_name())
  {
    db.open();
    db.query(schema).create().table<airplane>("airplane").execute();
  }

  ~StatementTestFixture()
  {
    drop_table_if_exists("airplane");
  }

protected:
  matador::sql::connection db;
  matador::sql::schema schema;

  std::vector<matador::object_ptr<airplane>> planes{
    matador::test::detail::make_object_ptr<airplane>(1, "Airbus", "A380"),
    matador::test::detail::make_object_ptr<airplane>(2, "Boeing", "707"),
    matador::test::detail::make_object_ptr<airplane>(3, "Boeing", "747")
  };

private:
  void drop_table_if_exists(const std::string &table_name) {
    if (db.exists(table_name)) {
      db.query(schema).drop().table(table_name).execute();
    }
  }

};

TEST_CASE_METHOD(StatementTestFixture, "Create prepared statement", "[statement]")
{
  using namespace matador::utils;
  schema.attach<airplane>("airplane");
  table ap{"airplane"};
  SECTION("Insert with prepared statement and placeholder") {
    auto stmt = db.query(schema).insert()
    .into("airplane", column_generator::generate<airplane>(schema, true))
    .values<airplane>()
    .prepare();

    for (const auto &plane: planes) {
      auto res = stmt.bind(*plane).execute();
      REQUIRE(res == 1);
      stmt.reset();
    }

    auto result = db.query(schema)
      .select(column_generator::generate<airplane>(schema, true))
      .from(ap)
      .fetch_all<airplane>();

    size_t index{0};
    for (const auto &i: result) {
      REQUIRE(i.id == planes[index]->id);
      REQUIRE(i.brand == planes[index]->brand);
      REQUIRE(i.model == planes[index++]->model);
    }
  }

  SECTION("Select with prepared statement") {
    for (const auto &plane: planes) {
      auto res = db.query(schema)
        .insert()
        .into("airplane", column_generator::generate<airplane>(schema, true))
        .values(*plane)
        .execute();
      REQUIRE(res == 1);
    }

    auto stmt = db.query(schema)
      .select(column_generator::generate<airplane>(schema, true))
      .from(ap)
      .where("brand"_col == _)
      .prepare();

    stmt.bind(0, "Airbus");

    auto result = stmt.fetch<airplane>();

    for (const auto &i: result) {
      REQUIRE(i.id == planes[0]->id);
      REQUIRE(i.brand == planes[0]->brand);
      REQUIRE(i.model == planes[0]->model);
    }

    stmt.reset();

    stmt.bind(0, "Boeing");

    result = stmt.fetch<airplane>();

    size_t index{1};
    for (const auto &i: result) {
      REQUIRE(i.id == planes[index]->id);
      REQUIRE(i.brand == planes[index]->brand);
      REQUIRE(i.model == planes[index++]->model);
    }
  }
}