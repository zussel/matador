#include <catch2/catch_test_macros.hpp>

#include "matador/query/dialect/dialect.hpp"
#include "matador/query/criteria_evaluator.hpp"
#include "matador/query/internal/select_query_builder.hpp"
#include "matador/query/schema.hpp"

#include "models/airplane.hpp"
#include "models/department.hpp"
#include "models/flight.hpp"
#include "models/recipe.hpp"

using namespace matador::query;
using namespace matador::test;

namespace matador::query {
class dialect_builder {
public:
  static dialect make_default() { return {}; }
};
}

TEST_CASE("Select query builder reports unknown types", "[query][select_builder]") {
  schema scm;
  select_query_builder builder(scm);
  const auto result = builder.build<department>();
  REQUIRE(!result.is_ok());
}

TEST_CASE("Select query builder joins eager has_many relations", "[query][select_builder]") {
  schema scm;
  REQUIRE(scm.attach<department>("department").is_ok());
  REQUIRE(scm.attach<employee>("employee").is_ok());

  const auto dialect = dialect_builder::make_default();
  select_query_builder builder(scm);
  const auto result = builder.build<department>();
  REQUIRE(result.is_ok());
  const auto sql = result->str(dialect);
  REQUIRE(sql.find("LEFT JOIN") != std::string::npos);
  REQUIRE(sql.find("ORDER BY") != std::string::npos);
}

TEST_CASE("Select query builder can be reused", "[query][select_builder]") {
  schema scm;
  REQUIRE(scm.attach<department>("department").is_ok());
  REQUIRE(scm.attach<employee>("employee").is_ok());

  const auto dialect = dialect_builder::make_default();
  select_query_builder builder(scm);
  const auto first = builder.build<department>();
  const auto second = builder.build<department>();
  REQUIRE(first.is_ok());
  REQUIRE(second.is_ok());
  REQUIRE(first->str(dialect) == second->str(dialect));
}

TEST_CASE("Select query builder joins many to many relations", "[query][select_builder]") {
  schema scm;
  REQUIRE(scm.attach<ingredient>("ingredient").is_ok());
  REQUIRE(scm.attach<recipe>("recipe").is_ok());

  const auto dialect = dialect_builder::make_default();
  select_query_builder builder(scm);
  const auto result = builder.build<ingredient>();
  REQUIRE(result.is_ok());
  const auto sql = result->str(dialect);
  INFO(sql);
  REQUIRE(sql.find("recipe_ingredients") != std::string::npos);
}

TEST_CASE("Select query builder rebinds criteria to the root alias", "[query][select_builder]") {
  schema scm;
  REQUIRE(scm.attach<department>("department").is_ok());
  REQUIRE(scm.attach<employee>("employee").is_ok());
  const auto info = scm.find(typeid(department));
  REQUIRE(info != scm.end());

  const auto dialect = dialect_builder::make_default();
  select_query_builder builder(scm);
  const auto result = builder.build<department>(*info->info().table()->primary_key_column() == 1);
  REQUIRE(result.is_ok());
  const auto sql = result->str(dialect);
  INFO(sql);
  REQUIRE(sql.find("WHERE \"t01\".\"id\"") != std::string::npos);
}

TEST_CASE("Create sql query data for entity with eager has one", "[query][entity][builder]") {
  schema scm;
  auto result = scm.attach<airplane>("airplanes")
    .and_then( [&scm] { return scm.attach<flight>("flights"); } );
  REQUIRE(result);

  select_query_builder eqb(scm);

  const auto it = scm.find(typeid(flight));
  REQUIRE(it != scm.end());
  const auto* col = it->info().table()->find_column("id");
  REQUIRE(col);
  auto q = eqb.build<flight>(*col == _);
  REQUIRE(q.is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto sql = q->str(dialect);
  const std::string expected_sql = R"(SELECT "t01"."id", "t02"."id", "t02"."brand", "t02"."model", "t01"."pilot_name" FROM "flights" "t01" LEFT JOIN "airplanes" "t02" ON "t01"."airplane_id" = "t02"."id" WHERE "t01"."id" = ? ORDER BY "t01"."id" ASC)";
  REQUIRE(expected_sql == sql);

  const auto& data = eqb.query_data();
  REQUIRE(data.root_table->table_name() == "flights");
  REQUIRE(data.joins.size() == 1);
  const auto flights = table("flights").as("t01");
  const auto airplanes = table("airplanes").as("t02");
  const std::vector expected_columns {
      column::make_plain(&flights, "id", "c01", matador::utils::basic_type::UInt32),
      column::make_plain(&airplanes, "id", "c02", matador::utils::basic_type::UInt32),
      column::make_plain(&airplanes, "brand", "c03", matador::utils::basic_type::Varchar),
      column::make_plain(&airplanes, "model", "c04", matador::utils::basic_type::Varchar),
      column::make_plain(&flights, "pilot_name", "c05", matador::utils::basic_type::Varchar),
    };
  REQUIRE(data.columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(data.columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
      { "airplanes", R"("t01"."airplane_id" = "t02"."id")"}
  };

  query_context qc;
  size_t index{0};
  criteria_evaluator evaluator(dialect, qc);
  for (const auto &[join_table, condition] : data.joins) {
    REQUIRE(join_table->table_name() == expected_join_data[index].first);
    // REQUIRE(evaluator.evaluate(*condition) == expected_join_data[index].second);
    ++index;
  }

  // REQUIRE(data.where_clause.get());
  // auto cond = evaluator.evaluate(*data.where_clause);
  // REQUIRE(cond == R"("t01"."id" = ?)");
}
