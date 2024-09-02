#include <catch2/catch_test_macros.hpp>

#include <matador/sql/column_definition.hpp>
#include <matador/sql/condition.hpp>
#include <matador/sql/connection.hpp>
#include <matador/sql/dialect_builder.hpp>
#include <matador/sql/query.hpp>
#include <matador/sql/table.hpp>

#include "models/author.hpp"
#include "models/book.hpp"

using namespace matador::sql;
using namespace matador::utils;

TEST_CASE("Create table sql statement string", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  auto result = q.create().table({"person"}, {
  make_pk_column<unsigned long>("id"),
  make_column<std::string>("name", 255),
  make_column<unsigned short>("age")
  }).build();

  REQUIRE(result.sql == R"##(CREATE TABLE "person" ("id" BIGINT NOT NULL, "name" VARCHAR(255) NOT NULL, "age" INTEGER NOT NULL, CONSTRAINT PK_person PRIMARY KEY (id)))##");
  REQUIRE(result.table.name == "person");

  result = q.create().table("person", {
  make_pk_column<unsigned long>("id"),
  make_column<std::string>("name", {255, constraints::UNIQUE}, null_option::NOT_NULL),
  make_column<unsigned short>("age"),
  make_fk_column<unsigned long>("address", "address", "id")
  }).build();

  REQUIRE(result.sql == R"##(CREATE TABLE "person" ("id" BIGINT NOT NULL, "name" VARCHAR(255) NOT NULL UNIQUE, "age" INTEGER NOT NULL, "address" BIGINT NOT NULL, CONSTRAINT PK_person PRIMARY KEY (id), CONSTRAINT FK_person_address FOREIGN KEY (address) REFERENCES address(id)))##");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Drop table sql statement string", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  const auto result = q.drop().table("person").build();

  REQUIRE(result.sql == R"(DROP TABLE "person")");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Select sql statement string", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  const auto result = q.select({"id", "name", "age"}).from("person").build();

  REQUIRE(result.sql == R"(SELECT "id", "name", "age" FROM "person")");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Insert sql statement string", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  const auto result = q.insert().into("person", {
  "id", "name", "age"
  }).values({7UL, "george", 65U}).build();

  REQUIRE(result.sql == R"(INSERT INTO "person" ("id", "name", "age") VALUES (7, 'george', 65))");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Update sql statement string", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  const auto result = q.update("person").set({
                                             {"id",   7UL},
                                             {"name", "george"},
                                             {"age",  65U}
                                             }).build();

  REQUIRE(result.sql == R"(UPDATE "person" SET "id"=7, "name"='george', "age"=65)");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Update limit sql statement", "[query][update][limit]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  const auto result = q.update("person")
  .set({{"id",   7UL}, {"name", "george"}, {"age",  65U}})
  .where("name"_col == "george")
  .order_by("id"_col).asc()
  .limit(2)
  .build();

  REQUIRE(result.sql == R"(UPDATE "person" SET "id"=7, "name"='george', "age"=65 WHERE "name" = 'george' ORDER BY "id" ASC LIMIT 2)");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Delete sql statement string", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  const auto result = q.remove().from("person").build();

  REQUIRE(result.sql == R"(DELETE FROM "person")");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Delete limit sql statement", "[query][delete][limit]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  const auto result = q.remove()
  .from("person")
  .where("name"_col == "george")
  .order_by("id"_col).asc()
  .limit(2)
  .build();

  REQUIRE(result.sql == R"(DELETE FROM "person" WHERE "name" = 'george' ORDER BY "id" ASC LIMIT 2)");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Select sql statement string with where clause", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  auto result = q.select({"id", "name", "age"})
  .from("person")
  .where("id"_col == 8 && "age"_col > 50)
  .build();

  REQUIRE(result.sql == R"(SELECT "id", "name", "age" FROM "person" WHERE ("id" = 8 AND "age" > 50))");
  REQUIRE(result.table.name == "person");

  result = q.select({"id", "name", "age"})
  .from("person")
  .where("id"_col == _ && "age"_col > 50)
  .build();

  REQUIRE(result.sql == R"(SELECT "id", "name", "age" FROM "person" WHERE ("id" = ? AND "age" > 50))");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Insert sql statement with placeholder", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  const auto result = q.insert().into("person", {
  "id", "name", "age"
  }).values({_, _, _}).build();

  REQUIRE(result.sql == R"(INSERT INTO "person" ("id", "name", "age") VALUES (?, ?, ?))");
  REQUIRE(result.table.name == "person");
  REQUIRE(result.bind_vars.size() == 3);
}

TEST_CASE("Select sql statement string with order by", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  const auto result = q.select({"id", "name", "age"})
  .from("person")
  .order_by("name").asc()
  .build();

  REQUIRE(result.sql == R"(SELECT "id", "name", "age" FROM "person" ORDER BY "name" ASC)");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Select sql statement string with group by", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  const auto result = q.select({"id", "name", "age"})
  .from("person")
  .group_by("age")
  .build();

  REQUIRE(result.sql == R"(SELECT "id", "name", "age" FROM "person" GROUP BY "age")");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Select sql statement string with offset and limit", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  const auto result = q.select({"id", "name", "age"})
  .from("person")
  .order_by("id"_col).asc()
  .limit(20)
  .offset(10)
  .build();

  REQUIRE(result.sql == R"(SELECT "id", "name", "age" FROM "person" ORDER BY "id" ASC LIMIT 20 OFFSET 10)");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Create, insert and select a blob column", "[query][blob]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  auto result = q.create().table("person", {
  make_pk_column<unsigned long>("id"),
  make_column<std::string>("name", 255),
  make_column<blob>("data")
  }).build();

  REQUIRE(result.sql == R"##(CREATE TABLE "person" ("id" BIGINT NOT NULL, "name" VARCHAR(255) NOT NULL, "data" BLOB NOT NULL, CONSTRAINT PK_person PRIMARY KEY (id)))##");
  REQUIRE(result.table.name == "person");

  result = q.insert().into("person", {
  "id", "name", "data"
  }).values({7UL, "george", blob{1, 'A', 3, 4}}).build();

  REQUIRE(result.sql == R"(INSERT INTO "person" ("id", "name", "data") VALUES (7, 'george', X'01410304'))");
  REQUIRE(result.table.name == "person");

  result = q.select({"id", "name", "data"}).from("person").build();

  REQUIRE(result.sql == R"(SELECT "id", "name", "data" FROM "person")");
  REQUIRE(result.table.name == "person");
}

TEST_CASE("Select statement with join_left", "[query][join_left]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  query q(noop, scm);
  const auto result = q.select({"f.id", "ap.brand", "f.pilot_name"})
  .from({"flight", "f"})
  .join_left({"airplane", "ap"})
  .on("f.airplane_id"_col == "ap.id"_col)
  .build();

  REQUIRE(result.sql == R"(SELECT "f"."id", "ap"."brand", "f"."pilot_name" FROM "flight" AS "f" INNER JOIN "airplane" AS "ap" ON "f"."airplane_id" = "ap"."id")");
  REQUIRE(result.table.name == "flight");
}

TEST_CASE("Select statement with aliased columns", "[query][select][alias]") {
  using namespace matador::test;
  connection noop("noop://noop.db");
  schema scm("noop");
  scm.attach<author>("authors");
  scm.attach<book>("books");

  query q(noop, scm);
  const auto result = q.select<author>().from("authors"_tab.as("A")).build();

}