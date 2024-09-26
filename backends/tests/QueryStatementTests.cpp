#include <catch2/catch_test_macros.hpp>

#include "matador/sql/column.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/query.hpp"

#include "models/person.hpp"

#include "QueryFixture.hpp"

#include <algorithm>
#include <vector>

using namespace matador::sql;
using namespace matador::test;

TEST_CASE_METHOD(QueryFixture, "Test create statement", "[query][statement][create]") {
  schema.attach<matador::test::person>("person");
  auto stmt = query::create()
    .table<matador::test::person>("person", schema)
    .prepare(db);

  auto res = stmt.execute();
  REQUIRE(res == 0);
  tables_to_drop.emplace("person");

  REQUIRE(db.exists("person"));
  const std::vector<std::string> cols = {"id", "name", "age", "image"};
  const auto fields = db.describe("person");

  for (const auto &fld : fields) {
    REQUIRE(std::find(cols.begin(), cols.end(), fld.name()) != cols.end());
  }
}

TEST_CASE_METHOD(QueryFixture, "Test insert statement", "[query][statement][insert]") {
  using namespace matador::test;

  schema.attach<matador::test::person>("person");
  auto stmt = query::create()
    .table<matador::test::person>("person", schema)
    .prepare(db);

  auto res = stmt.execute();
  REQUIRE(res == 0);
  tables_to_drop.emplace("person");

  REQUIRE(db.exists("person"));

  person george{1, "george", 45, {1,2,3,4}};

  stmt = query::insert()
    .into<person>("person", schema)
    .values<person>()
    .prepare(db);

  res = stmt.bind(george)
    .execute();
  REQUIRE(res == 1);

  auto row = query::select<person>(schema)
    .from("person")
    .fetch_one<person>(db);

  REQUIRE(row != nullptr);
  REQUIRE(row->id == 1);
  REQUIRE(row->name == "george");
  REQUIRE(row->age == 45);
  REQUIRE(row->image == matador::utils::blob{1,2,3,4});
}

TEST_CASE_METHOD(QueryFixture, "Test update statement", "[query][statement][update]") {
  using namespace matador::test;

  schema.attach<matador::test::person>("person");
  auto stmt = query::create()
    .table<matador::test::person>("person", schema)
    .prepare(db);

  auto res = stmt.execute();
  REQUIRE(res == 0);
  tables_to_drop.emplace("person");

  REQUIRE(db.exists("person"));

  person george{1, "george", 45, {1,2,3,4}};

  stmt = query::insert()
    .into<person>("person", schema)
    .values<person>()
    .prepare(db);

  res = stmt.bind(george)
    .execute();
  REQUIRE(res == 1);

  auto row = query::select<person>(schema)
    .from("person")
    .fetch_one<person>(db);

  REQUIRE(row != nullptr);
  REQUIRE(row->id == 1);
  REQUIRE(row->name == "george");
  REQUIRE(row->age == 45);
  REQUIRE(row->image == matador::utils::blob{1,2,3,4});

  george.age = 36;
  george.image = {5,6,7,8};
  stmt = query::update("person")
    .set<person>()
    .where("id"_col == matador::utils::_)
    .prepare(db);

  res = stmt.bind(george)
    .bind(4, george.id)
    .execute();
  REQUIRE(res == 1);

  row = query::select<person>(schema)
    .from("person")
    .fetch_one<person>(db);

  REQUIRE(row != nullptr);
  REQUIRE(row->id == 1);
  REQUIRE(row->name == "george");
  REQUIRE(row->age == 36);
  REQUIRE(row->image == matador::utils::blob{5,6,7,8});
}

TEST_CASE_METHOD(QueryFixture, "Test delete statement", "[query][statement][delete]") {
  using namespace matador::test;

  schema.attach<matador::test::person>("person");
  auto stmt = query::create()
    .table<matador::test::person>("person", schema)
    .prepare(db);

  auto res = stmt.execute();
  REQUIRE(res == 0);
  tables_to_drop.emplace("person");

  REQUIRE(db.exists("person"));

  stmt = query::insert()
    .into<person>("person", schema)
    .values<person>()
    .prepare(db);

  std::vector<person> peoples {
    {1,"george", 45, {1,2,3,4}},
    {2,"jane", 36, {1,2,3,4}},
    {3,"lukas", 68, {1,2,3,4}},
    {4,"merlin", 99, {1,2,3,4}}
  };

  for (const auto &p : peoples) {
    res = stmt.bind(p)
      .execute();
    REQUIRE(res == 1);
    stmt.reset();
  }

  auto select_stmt = query::select<person>(schema)
    .from("person")
    .where("name"_col == matador::utils::_)
    .prepare(db);

  auto rows = select_stmt.bind(0, "jane")
    .fetch<person>();

  for (const auto &r : rows) {
    constexpr size_t index = 1;
    REQUIRE(r.id == peoples[index].id);
    REQUIRE(r.name == peoples[index].name);
    REQUIRE(r.age == peoples[index].age);
    REQUIRE(r.image == peoples[index].image);
  }

  stmt = query::remove()
    .from("person")
    .where("name"_col == matador::utils::_)
    .prepare(db);

  res = stmt.bind(0, "jane")
    .execute();
  REQUIRE(res == 1);

  select_stmt.reset();
  auto row = select_stmt.bind(0, "jane")
    .fetch_one<person>();

  REQUIRE(row == nullptr);

  stmt.reset();
  res = stmt.bind(0, "merlin")
    .execute();
  REQUIRE(res == 1);

  select_stmt.reset();
  row = select_stmt.bind(0, "merlin")
    .fetch_one<person>();

  REQUIRE(row == nullptr);
}

TEST_CASE_METHOD(QueryFixture, "Test reuse prepared statement", "[query][statement][reuse]") {
  using namespace matador::test;

  schema.attach<matador::test::person>("person");
  auto stmt = query::create()
    .table<matador::test::person>("person", schema)
    .prepare(db);

  auto res = stmt.execute();
  REQUIRE(res == 0);
  tables_to_drop.emplace("person");

  REQUIRE(db.exists("person"));

  stmt = query::insert()
    .into<person>("person", schema)
    .values<person>()
    .prepare(db);

  std::vector<person> peoples {
    {1,"george", 45, {1,2,3,4}},
    {2,"jane", 36, {1,2,3,4}},
    {3,"lukas", 68, {1,2,3,4}},
    {4,"merlin", 99, {1,2,3,4}}
  };

  for (const auto &p : peoples) {
    res = stmt.bind(p)
      .execute();
    REQUIRE(res == 1);
    stmt.reset();
  }

  stmt = query::select<person>(schema)
    .from("person")
    .prepare(db);

  auto rows = stmt.fetch<person>();

  size_t index = 0;
  for (const auto &r : rows) {
    REQUIRE(r.id == peoples[index].id);
    REQUIRE(r.name == peoples[index].name);
    REQUIRE(r.age == peoples[index].age);
    REQUIRE(r.image == peoples[index].image);
    ++index;
  }

  stmt.reset();

  rows = stmt.fetch<person>();

  index = 0;
  for (const auto &r : rows) {
    REQUIRE(r.id == peoples[index].id);
    REQUIRE(r.name == peoples[index].name);
    REQUIRE(r.age == peoples[index].age);
    REQUIRE(r.image == peoples[index].image);
    ++index;
  }
}