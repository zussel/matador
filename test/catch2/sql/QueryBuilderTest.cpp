#include <catch2/catch_test_macros.hpp>

#include <matador/sql/column_definition.hpp>
#include <matador/sql/condition.hpp>
#include <matador/sql/connection.hpp>
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
  
  auto result = query::create().table({"person"}, {
  make_pk_column<unsigned long>("id"),
  make_column<std::string>("name", 255),
  make_column<unsigned short>("age")
  }).str(noop);

  REQUIRE(result == R"##(CREATE TABLE "person" ("id" BIGINT NOT NULL, "name" VARCHAR(255) NOT NULL, "age" INTEGER NOT NULL, CONSTRAINT PK_person PRIMARY KEY (id)))##");

  result = query::create().table("person", {
  make_pk_column<unsigned long>("id"),
  make_column<std::string>("name", {255, constraints::UNIQUE}, null_option::NOT_NULL),
  make_column<unsigned short>("age"),
  make_fk_column<unsigned long>("address", "address", "id")
  }).str(noop);

  REQUIRE(result == R"##(CREATE TABLE "person" ("id" BIGINT NOT NULL, "name" VARCHAR(255) NOT NULL UNIQUE, "age" INTEGER NOT NULL, "address" BIGINT NOT NULL, CONSTRAINT PK_person PRIMARY KEY (id), CONSTRAINT FK_person_address FOREIGN KEY (address) REFERENCES address(id)))##");
}

TEST_CASE("Drop table sql statement string", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  const auto result = query::drop().table("person").str(noop);

  REQUIRE(result == R"(DROP TABLE "person")");
}

TEST_CASE("Select sql statement string", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  const auto result = query::select({"id", "name", "age"}).from("person").str(noop);

  REQUIRE(result == R"(SELECT "id", "name", "age" FROM "person")");
}

TEST_CASE("Insert sql statement string", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  const auto result = query::insert().into("person", {
  "id", "name", "age"
  }).values({7UL, "george", 65U}).str(noop);

  REQUIRE(result == R"(INSERT INTO "person" ("id", "name", "age") VALUES (7, 'george', 65))");
}

TEST_CASE("Update sql statement string", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  const auto result = query::update("person").set({
                                             {"id",   7UL},
                                             {"name", "george"},
                                             {"age",  65U}
                                             }).str(noop);

  REQUIRE(result == R"(UPDATE "person" SET "id"=7, "name"='george', "age"=65)");
}

TEST_CASE("Update limit sql statement", "[query][update][limit]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  const auto result = query::update("person")
  .set({{"id",   7UL}, {"name", "george"}, {"age",  65U}})
  .where("name"_col == "george")
  .order_by("id"_col).asc()
  .limit(2)
  .str(noop);

  REQUIRE(result == R"(UPDATE "person" SET "id"=7, "name"='george', "age"=65 WHERE "name" = 'george' ORDER BY "id" ASC LIMIT 2)");
}

TEST_CASE("Delete sql statement string", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  const auto result = query::remove().from("person").str(noop);

  REQUIRE(result == R"(DELETE FROM "person")");
}

TEST_CASE("Delete limit sql statement", "[query][delete][limit]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  const auto result = query::remove()
  .from("person")
  .where("name"_col == "george")
  .order_by("id"_col).asc()
  .limit(2)
  .str(noop);

  REQUIRE(result == R"(DELETE FROM "person" WHERE "name" = 'george' ORDER BY "id" ASC LIMIT 2)");
}

TEST_CASE("Select sql statement string with where clause", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  auto result = query::select({"id", "name", "age"})
  .from("person")
  .where("id"_col == 8 && "age"_col > 50)
  .str(noop);

  REQUIRE(result == R"(SELECT "id", "name", "age" FROM "person" WHERE ("id" = 8 AND "age" > 50))");

  result = query::select({"id", "name", "age"})
  .from("person")
  .where("id"_col == _ && "age"_col > 50)
  .str(noop);

  REQUIRE(result == R"(SELECT "id", "name", "age" FROM "person" WHERE ("id" = ? AND "age" > 50))");
}

TEST_CASE("Insert sql statement with placeholder", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  const auto result = query::insert().into("person", {
  "id", "name", "age"
  }).values({_, _, _}).str(noop);

  REQUIRE(result == R"(INSERT INTO "person" ("id", "name", "age") VALUES (?, ?, ?))");
}

TEST_CASE("Select sql statement string with order by", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  const auto result = query::select({"id", "name", "age"})
  .from("person")
  .order_by("name"_col).asc()
  .str(noop);

  REQUIRE(result == R"(SELECT "id", "name", "age" FROM "person" ORDER BY "name" ASC)");
}

TEST_CASE("Select sql statement string with group by", "[query]")
{
  connection noop("noop://noop.db");
  const schema scm("noop");
  
  const auto result = query::select({"id", "name", "age"})
  .from("person")
  .group_by("age"_col)
  .str(noop);

  REQUIRE(result == R"(SELECT "id", "name", "age" FROM "person" GROUP BY "age")");
}

TEST_CASE("Select sql statement string with offset and limit", "[query]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  const auto result = query::select({"id", "name", "age"})
  .from("person")
  .order_by("id"_col).asc()
  .limit(20)
  .offset(10)
  .str(noop);

  REQUIRE(result == R"(SELECT "id", "name", "age" FROM "person" ORDER BY "id" ASC LIMIT 20 OFFSET 10)");
}

TEST_CASE("Create, insert and select a blob column", "[query][blob]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  auto result = query::create().table("person", {
  make_pk_column<unsigned long>("id"),
  make_column<std::string>("name", 255),
  make_column<blob>("data")
  }).str(noop);

  REQUIRE(result == R"##(CREATE TABLE "person" ("id" BIGINT NOT NULL, "name" VARCHAR(255) NOT NULL, "data" BLOB NOT NULL, CONSTRAINT PK_person PRIMARY KEY (id)))##");

  result = query::insert().into("person", {
  "id", "name", "data"
  }).values({7UL, "george", blob{1, 'A', 3, 4}}).str(noop);

  REQUIRE(result == R"(INSERT INTO "person" ("id", "name", "data") VALUES (7, 'george', X'01410304'))");

  result = query::select({"id", "name", "data"}).from("person").str(noop);

  REQUIRE(result == R"(SELECT "id", "name", "data" FROM "person")");
}

TEST_CASE("Select statement with join_left", "[query][join_left]")
{
  connection noop("noop://noop.db");
  schema scm("noop");
  
  const auto result = query::select({"f.id", "ap.brand", "f.pilot_name"})
  .from({"flight", "f"})
  .join_left({"airplane", "ap"})
  .on("f.airplane_id"_col == "ap.id"_col)
  .str(noop);

  REQUIRE(result == R"(SELECT "f"."id", "ap"."brand", "f"."pilot_name" FROM "flight" "f" LEFT JOIN "airplane" "ap" ON "f"."airplane_id" = "ap"."id")");
}

TEST_CASE("Select statement with aliased columns", "[query][select][alias]") {
  using namespace matador::test;
  connection noop("noop://noop.db");
  schema scm("noop");
  scm.attach<author>("authors");
  scm.attach<book>("books");

  
  const auto result = query::select<author>(scm)
    .from("authors"_tab.as("T01"))
    .str(noop);

  const auto expected_sql = R"(SELECT "T01"."id", "T01"."first_name", "T01"."last_name", "T01"."date_of_birth", "T01"."year_of_birth", "T01"."distinguished" FROM "authors" "T01")";

  REQUIRE(result == expected_sql);
}