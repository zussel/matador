#include <catch2/catch_test_macros.hpp>

#include "matador/query/criteria.hpp"
#include "matador/query/criteria_evaluator.hpp"
#include "matador/query/query.hpp"
#include "matador/query/dialect_builder.hpp"
#include "matador/query/placeholder.hpp"

using namespace matador::query;
using namespace matador::utils;

class CriteriaFixture {
protected:
  dialect dlc = dialect_builder::builder()
    .create()
    .build();
  query_context ctx;
};

TEST_CASE_METHOD(CriteriaFixture, "Test binary criteria", "[criteria][binary]") {
  const auto name_col = "name"_col;

  REQUIRE(name_col.name() == "name");

  auto clause = name_col !=  "george";

  criteria_evaluator evaluator(dlc, ctx);
  auto str = evaluator.evaluate(*clause);

  REQUIRE(str == "\"name\" <> 'george'");

  clause = "age"_col != 9;
  str = evaluator.evaluate(*clause);
  REQUIRE(str == "\"age\" <> 9");

  clause = "age"_col != _;
  str = evaluator.evaluate(*clause);

  REQUIRE(str == "\"age\" <> ?");
}

TEST_CASE_METHOD(CriteriaFixture, "Test not criteria", "[criteria][not]") {
  const auto name_col = "name"_col;
  auto clause = !(name_col == "Hans");

  criteria_evaluator evaluator(dlc, ctx);
  auto str = evaluator.evaluate(*clause);

  REQUIRE(str == "NOT (\"name\" = 'Hans')");

  clause = !(name_col == _);
  str = evaluator.evaluate(*clause);

  REQUIRE(str == "NOT (\"name\" = ?)");
}

TEST_CASE_METHOD(CriteriaFixture, "Test in criteria", "[criteria][in]") {
  const auto age_col = "age"_col;
  auto clause = age_col != 7 && in(age_col,  {7,5,5,8});

  criteria_evaluator evaluator(dlc, ctx);
  auto str = evaluator.evaluate(*clause);

  REQUIRE(str == "(\"age\" <> 7 AND \"age\" IN (7, 5, 5, 8))");

  clause = age_col != 7 && in(age_col,  {_, _, _});
  str = evaluator.evaluate(*clause);

  REQUIRE(str == "(\"age\" <> 7 AND \"age\" IN (?, ?, ?))");
}

TEST_CASE_METHOD(CriteriaFixture, "Test in query criteria", "[criteria][in query]") {
  const auto age_col = "age"_col;
  const auto name_col = "name"_col;

  query_context sub_ctx;
  sub_ctx.sql = R"(SELECT "name" FROM "test")";

  auto q = select({name_col}).from(table{"test"});

  auto clause = age_col != 7 && in(name_col, std::move(q));
}
TEST_CASE_METHOD(CriteriaFixture, "Test out criteria", "[criteria][out]") {
  const auto age_col = "age"_col;
  auto clause = age_col != 7 && out(age_col,  {7,5,5,8});

  criteria_evaluator evaluator(dlc, ctx);
  auto str = evaluator.evaluate(*clause);

  REQUIRE(str == R"(("age" <> 7 AND "age" NOT IN (7, 5, 5, 8)))");

  clause = age_col != 7 && out(age_col,  {_, _, _});
  str = evaluator.evaluate(*clause);

  REQUIRE(str == R"(("age" <> 7 AND "age" NOT IN (?, ?, ?)))");
}

TEST_CASE_METHOD(CriteriaFixture, "Test between criteria", "[criteria][between]") {
  const auto age_col = "age"_col;
  auto clause = age_col != 7 || between(age_col, 21, 30);

  criteria_evaluator evaluator(dlc, ctx);
  auto str = evaluator.evaluate(*clause);

  REQUIRE(str == R"(("age" <> 7 OR "age" BETWEEN 21 AND 30))");

  clause = age_col != 7 || between(age_col, _, _);
  str = evaluator.evaluate(*clause);

  REQUIRE(str == R"(("age" <> 7 OR "age" BETWEEN ? AND ?))");
}

TEST_CASE_METHOD(CriteriaFixture, "Test like criteria", "[criteria][like]") {
  const auto name_col = "name"_col;
  const auto clause = like(name_col, "%er%");

  criteria_evaluator evaluator(dlc, ctx);
  auto str = evaluator.evaluate(*clause);

  REQUIRE(str == "\"name\" LIKE '%er%'");
}