#include <catch2/catch_test_macros.hpp>

#include "matador/sql/connection.hpp"

#include "matador/query/criteria.hpp"
#include "matador/query/query.hpp"
#include "matador/query/table_column.hpp"

#include "models/person.hpp"
#include "models/model_metas.hpp"

#include "fixtures/QueryFixture.hpp"

#include <algorithm>
#include <vector>

using namespace matador::sql;
using namespace matador::object;
using namespace matador::query;
using namespace matador::query::meta;
using namespace matador::utils;
using namespace matador::test;

TEST_CASE_METHOD(QueryFixture, "Test create statement", "[query][statement][create]") {
  const auto obj = object_generator::generate<person>(repo.repo(), "persons");
  auto stmt = create()
    .table(obj->name())
    .columns(obj->attributes())
    .constraints(obj->constraints())
    .prepare(db);
  REQUIRE(stmt);

  auto res = stmt->execute();
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);
  tables_to_drop.emplace("persons");

  check_table_exists("persons");
  const std::vector<std::string> cols = {"id", "name", "age", "image"};
  const auto fields = db.describe("persons");
  REQUIRE(fields.is_ok());

  for (const auto &fld : *fields) {
    REQUIRE(std::find(cols.begin(), cols.end(), fld.name()) != cols.end());
  }
}

TEST_CASE_METHOD(QueryFixture, "Test insert statement", "[query][statement][insert]") {

  REQUIRE(repo.attach<person>("persons")
    .and_then([this] { return repo.create(db);}));

  check_table_exists(PERSON.table_name());

  person george{1, "george", 45, {1,2,3,4}};

  auto stmt = insert()
    .into(PERSON, {PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .values(generator::placeholders<person>())
    .prepare(db);
  REQUIRE(stmt);

  auto res = stmt->bind(george)
    .execute();
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto row = select({PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .from(PERSON)
    .fetch_one<person>(db);
  REQUIRE(row.is_ok());

  REQUIRE(*row);
  REQUIRE((*row)->id == 1);
  REQUIRE((*row)->name == "george");
  REQUIRE((*row)->age == 45);
  REQUIRE((*row)->image == matador::utils::blob_type_t{1,2,3,4});
}

TEST_CASE_METHOD(QueryFixture, "Test update statement", "[query][statement][update]") {
  REQUIRE(repo.attach<person>("persons")
    .and_then([this] { return repo.create(db);}));

  check_table_exists(PERSON.table_name());

  person george{1, "george", 45, {1,2,3,4}};

  auto stmt = insert()
    .into(PERSON, {PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .values(generator::placeholders<person>())
    .prepare(db);
  REQUIRE(stmt);

  auto res = stmt->bind(george)
    .execute();
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto row = select({PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .from(PERSON)
    .fetch_one<person>(db);
  REQUIRE(row.is_ok());

  REQUIRE(*row);
  REQUIRE((*row)->id == 1);
  REQUIRE((*row)->name == "george");
  REQUIRE((*row)->age == 45);
  REQUIRE((*row)->image == blob_type_t{1,2,3,4});

  george.age = 36;
  george.image = {5,6,7,8};
  stmt = update(PERSON)
    .set<person>()
    .where(PERSON.id == _)
    .prepare(db);
  REQUIRE(stmt);

  res = stmt->bind(george)
    .bind(4, george.id)
    .execute();
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  row = select({PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .from(PERSON)
    .fetch_one<person>(db);
  REQUIRE(row.is_ok());

  REQUIRE(*row);
  REQUIRE((*row)->id == 1);
  REQUIRE((*row)->name == "george");
  REQUIRE((*row)->age == 36);
  REQUIRE((*row)->image == blob_type_t{5,6,7,8});
}

TEST_CASE_METHOD(QueryFixture, "Test delete statement", "[query][statement][delete]") {
  REQUIRE(repo.attach<person>("persons")
    .and_then([this] { return repo.create(db);}));

  check_table_exists(PERSON.table_name());

  auto stmt = insert()
    .into(PERSON, {PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .values(generator::placeholders<person>())
    .prepare(db);
  REQUIRE(stmt);

  std::vector<person> peoples {
    {1,"george", 45, {1,2,3,4}},
    {2,"jane", 36, {1,2,3,4}},
    {3,"lukas", 68, {1,2,3,4}},
    {4,"merlin", 99, {1,2,3,4}}
  };

  for (const auto &p : peoples) {
    auto res = stmt->bind(p)
      .execute();
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
    stmt->reset();
  }

  auto select_stmt = select({PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .from(PERSON)
    .where(PERSON.name ==_)
    .prepare(db);
  REQUIRE(select_stmt);

  auto rows = select_stmt->bind(0, "jane")
    .fetch<person>();
  REQUIRE(rows.is_ok());

  for (const auto r : *rows) {
    constexpr size_t index = 1;
    REQUIRE(r->id == peoples[index].id);
    REQUIRE(r->name == peoples[index].name);
    REQUIRE(r->age == peoples[index].age);
    REQUIRE(r->image == peoples[index].image);
  }

  stmt = remove()
    .from(PERSON)
    .where(PERSON.name == _)
    .prepare(db);
  REQUIRE(stmt);

  auto res = stmt->bind(0, "jane")
    .execute();
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  select_stmt->reset();
  auto row = select_stmt->bind(0, "jane")
    .fetch_one<person>();
  REQUIRE(!row);

  stmt->reset();
  res = stmt->bind(0, "merlin")
    .execute();
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  select_stmt->reset();
  row = select_stmt->bind(0, "merlin")
    .fetch_one<person>();
  REQUIRE(!row);
}

TEST_CASE_METHOD(QueryFixture, "Test reuse prepared statement", "[query][statement][reuse]") {
  REQUIRE(repo.attach<matador::test::person>("persons")
    .and_then([this] { return repo.create(db);}));

  check_table_exists(PERSON.table_name());

  auto stmt = insert()
    .into(PERSON, {PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .values(generator::placeholders<person>())
    .prepare(db);
  REQUIRE(stmt);

  std::vector<person> peoples {
    {1,"george", 45, {1,2,3,4}},
    {2,"jane", 36, {1,2,3,4}},
    {3,"lukas", 68, {1,2,3,4}},
    {4,"merlin", 99, {1,2,3,4}}
  };

  for (const auto &p : peoples) {
    auto res = stmt->bind(p)
      .execute();
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
    stmt->reset();
  }

  stmt = select({PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .from(PERSON)
    .prepare(db);
  REQUIRE(stmt);

  auto rows = stmt->fetch<person>();
  REQUIRE(rows.is_ok());

  size_t index = 0;
  for (const auto &r : *rows) {
    REQUIRE(r->id == peoples[index].id);
    REQUIRE(r->name == peoples[index].name);
    REQUIRE(r->age == peoples[index].age);
    REQUIRE(r->image == peoples[index].image);
    ++index;
  }

  stmt->reset();

  rows = stmt->fetch<person>();
  REQUIRE(rows.is_ok());

  index = 0;
  for (const auto &r : *rows) {
    REQUIRE(r->id == peoples[index].id);
    REQUIRE(r->name == peoples[index].name);
    REQUIRE(r->age == peoples[index].age);
    REQUIRE(r->image == peoples[index].image);
    ++index;
  }
}