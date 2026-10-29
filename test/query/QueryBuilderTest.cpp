#include <catch2/catch_test_macros.hpp>

#include "fixtures/QueryFixture.hpp"

#include <matador/query/criteria.hpp>
#include <matador/query/builder/builder.hpp>
#include <matador/query/builder/query.hpp>
#include "matador/query/model/table.hpp"
#include "matador/query/model/column.hpp"

#include "matador/query/runtime/connection.hpp"

#include "matador/query/builder/placeholder.hpp"

using namespace matador::test;
using namespace matador::query;
using namespace matador::utils;

TEST_CASE_METHOD(QueryFixture, "Test alter table sql statement", "[query][alter][table]") {
  auto result = alter()
    .table("employees")
    .add_constraint("FK_employees_dep_id")
    .foreign_key("dep_id"_col)
    .references("departments", {"id"_col})
    .str(db->dialect());

  REQUIRE(result == R"(ALTER TABLE "employees" ADD CONSTRAINT FK_employees_dep_id FOREIGN KEY ("dep_id") REFERENCES departments ("id"))");
  result = alter()
    .table("employees")
    .drop_constraint("FK_employees_dep_id")
    .str(db->dialect());

  REQUIRE(result == R"(ALTER TABLE "employees" DROP CONSTRAINT FK_employees_dep_id)");
}

TEST_CASE_METHOD(QueryFixture, "Test create table sql statement string", "[query]") {
  auto result = create()
    .table({"person"})
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .constraints({
        constraint("PK_person").primary_key({"id"})
    })
    .str(db->dialect());

  REQUIRE(result == R"##(CREATE TABLE "person" ("id" BIGINT NOT NULL, "name" VARCHAR(255) NOT NULL, "age" INTEGER NOT NULL, CONSTRAINT PK_person PRIMARY KEY (id)))##");

  result = create()
    .table({"person"})
    .columns({
        col("id", basic_type::UInt32).primary_key().identity(),
        col("name", basic_type::Varchar, 255),
        col("age", basic_type::UInt16)
    })
    .str(db->dialect());

  REQUIRE(result == R"##(CREATE TABLE "person" ("id" BIGINT NOT NULL AUTO INCREMENT PRIMARY KEY, "name" VARCHAR(255) NOT NULL, "age" INTEGER NOT NULL))##");

  auto ctx = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255).unique(),
        col("age", basic_type::UInt16),
        col("address", basic_type::UInt32)
    })
    .constraints({
        constraint("PK_person").primary_key({"id"}),
        constraint("FK_person_address").foreign_key({"address"}).references("address", {"id"})
    })
    .compile(db->dialect());

  REQUIRE(ctx.sql == R"##(CREATE TABLE "person" ("id" BIGINT NOT NULL, "name" VARCHAR(255) NOT NULL UNIQUE, "age" INTEGER NOT NULL, "address" BIGINT NOT NULL, CONSTRAINT PK_person PRIMARY KEY (id), CONSTRAINT FK_person_address FOREIGN KEY (address) REFERENCES address (id)))##");
}

TEST_CASE_METHOD(QueryFixture, "Test drop table sql statement string", "[query]") {
  const auto result = drop()
    .table("person")
    .str(db->dialect());

  REQUIRE(result == R"(DROP TABLE "person")");
}

TEST_CASE_METHOD(QueryFixture, "Test select all columns with asterisk", "[query][select][asterisk]") {
  const auto result = select()
    .from("person")
    .str(db->dialect());

  REQUIRE(result == R"(SELECT * FROM "person")");
}

TEST_CASE_METHOD(QueryFixture, "Test select sql statement string", "[query]") {
  const auto result = select({"id", "name", "age"})
    .from("person")
    .str(db->dialect());

  REQUIRE(result == R"(SELECT "id", "name", "age" FROM "person")");
}

TEST_CASE_METHOD(QueryFixture, "Test insert sql statement string", "[query]") {
  const auto result = insert()
    .into("person", {
    "id", "name", "age"
    })
    .values({7U, "george", 65U})
    .str(db->dialect());

  REQUIRE(result == R"(INSERT INTO "person" ("id", "name", "age") VALUES (7, 'george', 65))");
}

TEST_CASE_METHOD(QueryFixture, "Test update sql statement string", "[query]") {
  auto result = update("person")
    .set("id", 7U)
    .set("name", "george")
    .set("age", 65U)
    .str(db->dialect());

  REQUIRE(result == R"(UPDATE "person" SET "id"=7, "name"='george', "age"=65)");

  result = update("person")
    .set("id", 7U)
    .set("name", "george")
    .set("age", 65U)
    .where("id"_col > 9)
    .order_by("id").asc()
    .limit(3)
    .offset(2)
    .str(db->dialect());

  REQUIRE(result == R"(UPDATE "person" SET "id"=7, "name"='george', "age"=65 WHERE "id" > 9 ORDER BY "id" ASC LIMIT 3 OFFSET 2)");
}

TEST_CASE_METHOD(QueryFixture, "Test update returning statement", "[query][update][returning]") {
    const auto result = update("person")
    .set("id", 7U)
    .set("name", "george")
    .set("age", 65U)
    .where("name"_col == "george")
      .returning("id"_col, "name"_col, "age"_col)
      .str(db->dialect());

    REQUIRE(result == R"(UPDATE "person" SET "id"=7, "name"='george', "age"=65 WHERE "name" = 'george' RETURNING "id", "name", "age")");
}

TEST_CASE_METHOD(QueryFixture, "Test update limit sql statement", "[query][update][limit]") {
  const auto result = update("person")
    .set("id", 7U)
    .set("name", "george")
    .set("age", 65U)
    .where("name"_col == "george")
    .order_by("id"_col).asc()
    .limit(2)
    .str(db->dialect());

  REQUIRE(result == R"(UPDATE "person" SET "id"=7, "name"='george', "age"=65 WHERE "name" = 'george' ORDER BY "id" ASC LIMIT 2)");
}

TEST_CASE_METHOD(QueryFixture, "Test delete sql statement string", "[query]") {
  const auto result = remove().from("person").str(db->dialect());

  REQUIRE(result == R"(DELETE FROM "person")");
}

TEST_CASE_METHOD(QueryFixture, "Test delete limit sql statement", "[query][delete][limit]") {
  const auto result = remove()
    .from("person")
    .where("name"_col == "george")
    .order_by("id"_col).asc()
    .limit(2)
    .str(db->dialect());

  REQUIRE(result == R"(DELETE FROM "person" WHERE "name" = 'george' ORDER BY "id" ASC LIMIT 2)");
}

TEST_CASE_METHOD(QueryFixture, "Test select sql statement string with where clause", "[query]") {
  auto result = select({"id", "name", "age"})
    .from("person")
    .where("id"_col == 8 && "age"_col > 50)
    .str(db->dialect());

  REQUIRE(result == R"(SELECT "id", "name", "age" FROM "person" WHERE ("id" = 8 AND "age" > 50))");

  result = select({"id", "name", "age"})
    .from("person")
    .where("id"_col == _ && "age"_col > 50)
    .str(db->dialect());

  REQUIRE(result == R"(SELECT "id", "name", "age" FROM "person" WHERE ("id" = ? AND "age" > 50))");
}

TEST_CASE_METHOD(QueryFixture, "Test insert sql statement with placeholder", "[query]") {
  auto result = insert()
    .into("person", {"id", "name", "age"})
    .values({_, _, _})
    .str(db->dialect());

  REQUIRE(result == R"(INSERT INTO "person" ("id", "name", "age") VALUES (?, ?, ?))");

  result = insert()
    .into("person", {"id", "name", "age"})
    .values({9, "george", _})
    .str(db->dialect());

  REQUIRE(result == R"(INSERT INTO "person" ("id", "name", "age") VALUES (9, 'george', ?))");
}

TEST_CASE_METHOD(QueryFixture, "Test insert sql statement with returning", "[query][insert][returning]") {
  const auto result = insert()
    .into("person", {"id", "name", "age"})
    .values({9, "george", _})
    .returning("id"_col, "name"_col, "age"_col)
    .str(db->dialect());

  REQUIRE(result == R"(INSERT INTO "person" ("id", "name", "age") VALUES (9, 'george', ?) RETURNING "id", "name", "age")");
}

TEST_CASE_METHOD(QueryFixture, "Test select sql statement string with order by", "[query]") {
  const auto result = select({"id", "name", "age"})
    .from("person")
    .order_by("name"_col).asc()
    .str(db->dialect());

  REQUIRE(result == R"(SELECT "id", "name", "age" FROM "person" ORDER BY "name" ASC)");
}

TEST_CASE_METHOD(QueryFixture, "Test select sql statement string with group by", "[query]") {
  const auto result = select({"id", "name", "age"})
    .from("person")
    .group_by("age"_col)
    .str(db->dialect());

  REQUIRE(result == R"(SELECT "id", "name", "age" FROM "person" GROUP BY "age")");
}

TEST_CASE_METHOD(QueryFixture, "Test select sql statement string with offset and limit", "[query]") {
  const auto result = select({"id", "name", "age"})
    .from("person")
    .order_by("id"_col).asc()
    .limit(20)
    .offset(10)
    .str(db->dialect());

  REQUIRE(result == R"(SELECT "id", "name", "age" FROM "person" ORDER BY "id" ASC LIMIT 20 OFFSET 10)");
}

TEST_CASE_METHOD(QueryFixture, "Test create, insert and select a blob column", "[query][blob]") {
  auto result = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("data", basic_type::Blob)
    })
    .constraints({
        constraint("PK_person").primary_key({"id"})
    })
    .str(db->dialect());

  REQUIRE(result == R"##(CREATE TABLE "person" ("id" BIGINT NOT NULL, "name" VARCHAR(255) NOT NULL, "data" BLOB NOT NULL, CONSTRAINT PK_person PRIMARY KEY (id)))##");

  result = insert()
    .into("person", {"id", "name", "data"})
    .values({7U, "george", blob_type_t{1, 'A', 3, 4}})
    .str(db->dialect());

  REQUIRE(result == R"(INSERT INTO "person" ("id", "name", "data") VALUES (7, 'george', X'01410304'))");

  result = select({"id", "name", "data"})
    .from("person")
    .str(db->dialect());

  REQUIRE(result == R"(SELECT "id", "name", "data" FROM "person")");
}

TEST_CASE_METHOD(QueryFixture, "Test select statement with join_left", "[query][statement][join_left]") {
  const auto ap = table("airplane").as("ap");
  const auto f = table("flight").as("f");
  const auto col1 = column::make_plain(&f, "airplane_id");
  const auto col2 = column::make_plain(&ap, "id");
  const auto result = select({"f.id", "ap.brand", "f.pilot_name"})
    .from(table{"flight"}.as("f"))
    .join_left(table{"airplane"}.as("ap"))
    .on(col1 == col2)
    .str(db->dialect());

  REQUIRE(result == R"(SELECT "f"."id", "ap"."brand", "f"."pilot_name" FROM "flight" "f" LEFT JOIN "airplane" "ap" ON "f"."airplane_id" = "ap"."id")");
}

// TEST_CASE_METHOD(QueryFixture, "Select statement with aliased columns", "[query][select][alias]") {
//   using namespace matador::test;
//   connection noop("noop://noop.db");
//   schema scm("noop");
//   scm.attach<author>("authors");
//   scm.attach<book>("books");
//
//
//   const auto result = select<author>(scm)
//     .from("authors"_tab.as("T01"))
//     .str(db->dialect());
//
//   const auto expected_sql = R"(SELECT "T01"."id", "T01"."first_name", "T01"."last_name", "T01"."date_of_birth", "T01"."year_of_birth", "T01"."distinguished" FROM "authors" "T01")";
//
//   REQUIRE(result == expected_sql);
// }