#include <catch2/catch_test_macros.hpp>

#include "matador/query/schema/schema.hpp"
#include "matador/query/criteria_evaluator.hpp"
#include "matador/query/dialect/dialect.hpp"
#include "matador/query/internal/select_query_builder.hpp"

#include "models/author.hpp"
#include "models/airplane.hpp"
#include "models/book.hpp"
#include "models/department.hpp"
#include "models/flight.hpp"
#include "models/northwind.hpp"
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
    ++index;
  }
}

TEST_CASE("Create sql query data for entity with eager belongs to", "[query][entity][builder]") {
  using namespace matador::test;

  schema scm;
  auto result = scm.attach<author>("authors")
    .and_then( [&scm] { return scm.attach<book>("books"); } );
  REQUIRE(result);

  select_query_builder eqb(scm);

  const auto it = scm.find(typeid(book));
  REQUIRE(it != scm.end());
  const auto* col = it->info().table()->find_column("id");
  REQUIRE(col);
  auto q = eqb.build<book>(*col == _);

  REQUIRE(q.is_ok());
  const auto dialect = dialect_builder::make_default();
  const auto sql = q->str(dialect);
  const std::string expected_sql = R"(SELECT "t01"."id", "t01"."title", "t02"."id", "t02"."first_name", "t02"."last_name", "t02"."date_of_birth", "t02"."year_of_birth", "t02"."distinguished", "t01"."published_in" FROM "books" "t01" LEFT JOIN "authors" "t02" ON "t01"."author_id" = "t02"."id" WHERE "t01"."id" = ? ORDER BY "t01"."id" ASC)";
  REQUIRE(expected_sql == sql);

  const auto& data = eqb.query_data();

  REQUIRE(data.root_table->table_name() == "books");
  REQUIRE(data.joins.size() == 1);
  const auto books = table("books").as("t01");
  const auto authors = table("authors").as("t02");
  const std::vector<column> expected_columns {
    column::make_plain(&books, "id", "c01", matador::utils::basic_type::UInt32),
    column::make_plain(&books, "title", "c02", matador::utils::basic_type::Varchar),
    column::make_plain(&authors, "id", "c03", matador::utils::basic_type::UInt32),
    column::make_plain(&authors, "first_name", "c04", matador::utils::basic_type::Varchar),
    column::make_plain(&authors, "last_name", "c05", matador::utils::basic_type::Varchar),
    column::make_plain(&authors, "date_of_birth", "c06", matador::utils::basic_type::Varchar),
    column::make_plain(&authors, "year_of_birth", "c07", matador::utils::basic_type::UInt16),
    column::make_plain(&authors, "distinguished", "c08", matador::utils::basic_type::Boolean),
    column::make_plain(&books, "published_in", "c09", matador::utils::basic_type::UInt16)
  };
  REQUIRE(data.columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(data.columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "authors", R"("t01"."author_id" = "t02"."id")"}
  };

  query_context qc;
  size_t index{0};
  criteria_evaluator evaluator(dialect, qc);
  for (const auto &join : data.joins) {
    REQUIRE(join.join_table->table_name() == expected_join_data[index].first);
    ++index;
  }
}

TEST_CASE("Create sql query data for entity with eager has many belongs to", "[query][entity][builder]") {
  using namespace matador::test;

  schema scm;
  auto result = scm.attach<product>("products")
    .and_then( [&scm] { return scm.attach<order_details>("order_details"); } )
    .and_then( [&scm] { return scm.attach<supplier>("suppliers"); } )
    .and_then( [&scm] { return scm.attach<category>("categories"); } )
    .and_then( [&scm] { return scm.attach<order>("orders"); } );
  REQUIRE(result);

  select_query_builder eqb(scm);

  const auto it = scm.find(typeid(order));
  REQUIRE(it != scm.end());
  const auto* col = it->table().find_column("order_id");
  REQUIRE(col);
  auto q = eqb.build<order>(*col == _);

  REQUIRE(q.is_ok());
  const auto dialect = dialect_builder::make_default();
  const auto sql = q->str(dialect);
  const std::string expected_sql = R"(SELECT "t01"."order_id", "t01"."order_date", "t01"."required_date", "t01"."shipped_date", "t01"."ship_via", "t01"."freight", "t01"."ship_name", "t01"."ship_address", "t01"."ship_city", "t01"."ship_region", "t01"."ship_postal_code", "t01"."ship_country", "t02"."order_details_id", "t02"."order_id" FROM "orders" "t01" LEFT JOIN "order_details" "t02" ON "t01"."order_id" = "t02"."order_id" WHERE "t01"."order_id" = ? ORDER BY "t01"."order_id" ASC)";
  REQUIRE(expected_sql == sql);

  const auto& data = eqb.query_data();

  REQUIRE(data.root_table->table_name() == "orders");
  REQUIRE(data.joins.size() == 1);
  const auto orders = table("orders").as("t01");
  const auto order_details = table("order_details").as("t02");
  const std::vector<column> expected_columns = {
    column::make_plain(&orders, "order_id", "c01", matador::utils::basic_type::UInt32),
    column::make_plain(&orders, "order_date", "c02", matador::utils::basic_type::Varchar),
    column::make_plain(&orders, "required_date", "c03", matador::utils::basic_type::Varchar),
    column::make_plain(&orders, "shipped_date", "c04", matador::utils::basic_type::Varchar),
    column::make_plain(&orders, "ship_via", "c05", matador::utils::basic_type::UInt32),
    column::make_plain(&orders, "freight", "c06", matador::utils::basic_type::UInt32),
    column::make_plain(&orders, "ship_name", "c07", matador::utils::basic_type::Varchar),
    column::make_plain(&orders, "ship_address", "c08", matador::utils::basic_type::Varchar),
    column::make_plain(&orders, "ship_city", "c09", matador::utils::basic_type::Varchar),
    column::make_plain(&orders, "ship_region", "c10", matador::utils::basic_type::Varchar),
    column::make_plain(&orders, "ship_postal_code", "c11", matador::utils::basic_type::Varchar),
    column::make_plain(&orders, "ship_country", "c12", matador::utils::basic_type::Varchar),
    column::make_plain(&order_details, "order_details_id", "c13", matador::utils::basic_type::UInt32),
    column::make_plain(&order_details, "order_id", "c14", matador::utils::basic_type::UInt32)
  };
  REQUIRE(data.columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(data.columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "order_details", R"("t01"."order_id" = "t02"."order_id")"}
  };

  query_context qc;
  size_t index{0};
  criteria_evaluator evaluator(dialect, qc);
  for (const auto &jd : data.joins) {
    REQUIRE(jd.join_table->table_name() == expected_join_data[index].first);
    ++index;
  }
}
