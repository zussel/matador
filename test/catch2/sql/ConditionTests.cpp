#include <catch2/catch_test_macros.hpp>

#include "matador/sql/condition.hpp"
#include "matador/sql/dialect_builder.hpp"

using namespace matador::sql;

class ConditionFixture {
protected:
  dialect dlc = dialect_builder::builder().build();
  query_context ctx;
};

TEST_CASE_METHOD(ConditionFixture, "Test column user defined literal", "[column][literal]") {
  const auto col = "name"_col;

  REQUIRE(col.name == "name");
}

TEST_CASE_METHOD(ConditionFixture, "Test logical condition", "[condition][logical]") {
  const auto name_col = "name"_col;

  REQUIRE(name_col.name == "name");

  auto cond1 = name_col !=  "george";

  auto clause = cond1.evaluate(dlc, ctx);
  REQUIRE(clause == "\"name\" <> 'george'");

  auto cond2 = "age"_col != 9;
  clause = cond2.evaluate(dlc, ctx);
  REQUIRE(clause == "\"age\" <> 9");
}

TEST_CASE_METHOD(ConditionFixture, "Test and condition", "[condition][and]") {
  const auto name_col = "name"_col;

  REQUIRE(name_col.name == "name");

  const auto cond = name_col == "Hans" && name_col == "Dieter";

  auto clause = cond.evaluate(dlc, ctx);
  REQUIRE(clause == "(\"name\" = 'Hans' AND \"name\" = 'Dieter')");
}

TEST_CASE_METHOD(ConditionFixture, "Test or condition", "[condition][or]") {
  const auto name_col = "name"_col;

  REQUIRE(name_col.name == "name");

  const auto cond = name_col == "Hans" || name_col == "Dieter";

  auto clause = cond.evaluate(dlc, ctx);
  REQUIRE(clause == "\"name\" = 'Hans' OR \"name\" = 'Dieter'");
}

TEST_CASE_METHOD(ConditionFixture, "Test not condition", "[condition][not]") {
  const auto name_col = "name"_col;

  REQUIRE(name_col.name == "name");

  const auto cond = !(name_col != "Hans");

  auto clause = cond.evaluate(dlc, ctx);
  REQUIRE(clause == "NOT (\"name\" <> 'Hans')");
}

TEST_CASE_METHOD(ConditionFixture, "Test in condition", "[condition][in]") {
  const auto age_col = "age"_col;

  REQUIRE(age_col.name == "age");
  auto cond = age_col != 7 && in(age_col,  {7,5,5,8});

  auto clause = cond.evaluate(dlc, ctx);
  REQUIRE(clause == "(\"age\" <> 7 AND \"age\" IN (7, 5, 5, 8))");

  cond = age_col != 7 && in(age_col,  {7});
  clause = cond.evaluate(dlc, ctx);
  REQUIRE(clause == "(\"age\" <> 7 AND \"age\" IN (7))");
}

TEST_CASE_METHOD(ConditionFixture, "Test in query condition", "[condition][in query]") {
  const auto age_col = "age"_col;
  const auto name_col = "name"_col;

  query_context sub_ctx;
  sub_ctx.sql = R"(SELECT "name" FROM "test")";

  auto cond = age_col != 7 && in(name_col,  std::move(sub_ctx));
  auto clause = cond.evaluate(dlc, ctx);
  REQUIRE(clause == "(\"age\" <> 7 AND \"name\" IN (SELECT \"name\" FROM \"test\"))");
}

TEST_CASE_METHOD(ConditionFixture, "Test between condition", "[condition][between]") {
  const auto age_col = "age"_col;

  auto cond = age_col != 7 && between(age_col, 21, 30);
  auto clause = cond.evaluate(dlc, ctx);
  REQUIRE(clause == "(\"age\" <> 7 AND \"age\" BETWEEN 21 AND 30)");
}

TEST_CASE_METHOD(ConditionFixture, "Test like condition", "[condition][like]") {
  const auto name_col = "name"_col;

  auto cond = like(name_col, "%er%");
  auto clause = cond.evaluate(dlc, ctx);
  REQUIRE(clause == "\"name\" LIKE '%er%'");
}
