#include <catch2/catch_test_macros.hpp>

#include "matador/sql/column.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/query_builder.hpp"

#include "models/person.hpp"

#include "connection.hpp"

#include <algorithm>
#include <vector>

class QueryStatementFixture {
public:
  QueryStatementFixture()
    : db(matador::test::connection::dns)
      , schema(db.dialect().default_schema_name()) {
    db.open();
  }

  ~QueryStatementFixture() {
    drop_table_if_exists("person");
  }

protected:
  matador::sql::connection db;
  matador::sql::schema schema;

private:
  void drop_table_if_exists(const std::string &table_name) const {
    if (db.exists(table_name)) {
      db.query(schema).drop().table(table_name).execute();
    }
  }
};

using namespace matador::sql;

TEST_CASE_METHOD(QueryStatementFixture, "Test create statement", "[query][statement][create]") {
  schema.attach<matador::test::person>("person");
  auto stmt = db.query(schema)
    .create()
    .table<matador::test::person>("person")
    .prepare();

  auto res = stmt.execute();
  REQUIRE(res == 0);

  REQUIRE(db.exists("person"));
  const std::vector<std::string> cols = {"id", "name", "age", "image"};
  const auto fields = db.describe("person");

  for (const auto &fld : fields) {
    REQUIRE(std::find(cols.begin(), cols.end(), fld.name()) != cols.end());
  }
}

TEST_CASE_METHOD(QueryStatementFixture, "Test insert statement", "[query][statement][insert]") {
  using namespace matador::test;

  schema.attach<matador::test::person>("person");
  auto stmt = db.query(schema)
    .create()
    .table<matador::test::person>("person")
    .prepare();

  auto res = stmt.execute();
  REQUIRE(res == 0);

  REQUIRE(db.exists("person"));

  person george{1, "george", 45, {1,2,3,4}};

  stmt = db.query(schema)
    .insert()
    .into<person>("person")
    .values<person>()
    .prepare();

  res = stmt.bind(george)
    .execute();
  REQUIRE(res == 1);

  auto row = db.query(schema)
    .select<person>()
    .from("person")
    .fetch_one<person>();

  REQUIRE(row != nullptr);
  REQUIRE(row->id == 1);
  REQUIRE(row->name == "george");
  REQUIRE(row->age == 45);
  REQUIRE(row->image == matador::utils::blob{1,2,3,4});
}

TEST_CASE_METHOD(QueryStatementFixture, "Test update statement", "[query][statement][update]") {
  using namespace matador::test;

  schema.attach<matador::test::person>("person");
  auto stmt = db.query(schema)
    .create()
    .table<matador::test::person>("person")
    .prepare();

  auto res = stmt.execute();
  REQUIRE(res == 0);

  REQUIRE(db.exists("person"));

  person george{1, "george", 45, {1,2,3,4}};

  stmt = db.query(schema)
    .insert()
    .into<person>("person")
    .values<person>()
    .prepare();

  res = stmt.bind(george)
    .execute();
  REQUIRE(res == 1);

  auto row = db.query(schema)
    .select<person>()
    .from("person")
    .fetch_one<person>();

  REQUIRE(row != nullptr);
  REQUIRE(row->id == 1);
  REQUIRE(row->name == "george");
  REQUIRE(row->age == 45);
  REQUIRE(row->image == matador::utils::blob{1,2,3,4});

  george.age = 36;
  george.image = {5,6,7,8};
  stmt = db.query(schema)
    .update("person")
    .set<person>()
    .where("id"_col == matador::utils::_)
    .prepare();

  res = stmt.bind(george)
    .bind(4, george.id)
    .execute();
  REQUIRE(res == 1);

  row = db.query(schema)
    .select<person>()
    .from("person")
    .fetch_one<person>();

  REQUIRE(row != nullptr);
  REQUIRE(row->id == 1);
  REQUIRE(row->name == "george");
  REQUIRE(row->age == 36);
  REQUIRE(row->image == matador::utils::blob{5,6,7,8});
}

TEST_CASE_METHOD(QueryStatementFixture, "Test delete statement", "[query][statement][delete]") {
  using namespace matador::test;

  schema.attach<matador::test::person>("person");
  auto stmt = db.query(schema)
    .create()
    .table<matador::test::person>("person")
    .prepare();

  auto res = stmt.execute();
  REQUIRE(res == 0);

  REQUIRE(db.exists("person"));

  stmt = db.query(schema)
    .insert()
    .into<person>("person")
    .values<person>()
    .prepare();

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

  stmt = db.query(schema)
    .select<person>()
    .from("person")
    .where("name"_col == matador::utils::_)
    .prepare();

  auto rows = stmt.bind(0, "jane")
    .fetch<person>();

  for (const auto &r : rows) {
    constexpr size_t index = 1;
    REQUIRE(r.id == peoples[index].id);
    REQUIRE(r.name == peoples[index].name);
    REQUIRE(r.age == peoples[index].age);
    REQUIRE(r.image == peoples[index].image);
  }

  stmt = db.query(schema)
    .remove()
    .from("person")
    .where("name"_col == matador::utils::_)
    .prepare();

  res = stmt.bind(0, "jane")
    .execute();
  REQUIRE(res == 1);

  stmt = db.query(schema)
    .select<person>()
    .from("person")
    .where("name"_col == matador::utils::_)
    .prepare();

  auto row = stmt.bind(0, "jane")
    .fetch_one<person>();

  REQUIRE(row == nullptr);
}
