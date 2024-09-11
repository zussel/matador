#include <catch2/catch_test_macros.hpp>

#include "matador/sql/connection.hpp"
#include "matador/sql/entity_query_builder.hpp"

#include "models/airplane.hpp"
#include "models/author.hpp"
#include "models/book.hpp"
#include "models/flight.hpp"
#include "models/recipe.hpp"
#include "models/order.hpp"
#include "models/student.hpp"

using namespace matador::sql;

TEST_CASE("Create sql query data for entity with eager has one", "[query][entity][builder]") {
  using namespace matador::test;
  connection db("noop://noop.db");
  schema scm("noop");
  scm.attach<airplane>("airplanes");
  scm.attach<flight>("flights");

  entity_query_builder eqb(scm);

  auto data = eqb.build<flight>(17);

  REQUIRE(data.is_ok());
  REQUIRE(data->root_table_name == "flights");
  REQUIRE(data->joins.size() == 1);
  const std::vector<column> expected_columns {
    { "flights", "id", "C01" },
    { "airplanes", "id", "C02" },
    { "airplanes", "brand", "C03" },
    { "airplanes", "model", "C04" },
    { "flights", "pilot_name", "C05" },
  };
  REQUIRE(data->columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(data->columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "airplanes", R"("flights"."airplane_id" = "airplanes"."id")"}
  };

  query_context qc;
  size_t index{0};
  for (const auto &jd : data->joins) {
    // REQUIRE(jd.join_table.name == expected_join_data[index].first);
    REQUIRE(jd.condition->evaluate(db.dialect(), qc) == expected_join_data[index].second);
    ++index;
  }

  REQUIRE(data->where_clause);
  auto cond = data->where_clause->evaluate(db.dialect(), qc);
  // REQUIRE(cond == R"("flights"."id" = 17)");

  const auto sql = db.query(scm)
    .select(data->columns)
    .from(data->root_table_name)
    .join_left(data->joins)
    .where(std::move(data->where_clause))
    .order_by({data->root_table_name, data->pk_column_})
    .asc()
    .build();

  // SELECT "T01"."id" AS C01, "T02"."id" AS C02,        "T02"."brand" AS C03,       "T02"."model" AS C04,       "pilot_name" AS C05 FROM "flights" "T01" INNER JOIN "airplanes" "T02" ON "T01"."airplane_id" = C01                  WHERE C01 = 17        ORDER BY C01 ASC
  // SELECT "id" AS C01,        "airplanes"."id" AS C02, "airplanes"."brand" AS C03, "airplanes"."model" AS C04, "pilot_name" AS C05 FROM "flights"       INNER JOIN "flights"   "T01" ON "flights"."airplane_id" = "airplanes"."id" WHERE "T01"."id" = 17 ORDER BY "flights"."id" ASC
  std::cout << sql.sql << "\n";
}

TEST_CASE("Create sql query data for entity with eager belongs to", "[query][entity][builder]") {
  using namespace matador::test;
  connection db("noop://noop.db");
  schema scm("noop");
  scm.attach<author>("authors");
  scm.attach<book>("books");

  entity_query_builder eqb(scm);

  auto data = eqb.build<book>(17);

  REQUIRE(data.is_ok());
  REQUIRE(data->root_table_name == "books");
  REQUIRE(data->joins.size() == 1);
  const std::vector<column> expected_columns {
    { "books", "id", "C01" },
    { "books", "title", "C02" },
    { "authors", "id", "C03" },
    { "authors", "first_name", "C04" },
    { "authors", "last_name", "C05" },
    { "authors", "date_of_birth", "C06" },
    { "authors", "year_of_birth", "C07" },
    { "authors", "distinguished", "C08" },
    { "books", "published_in", "C09" }
  };
  REQUIRE(data->columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(data->columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "authors", R"("books"."author_id" = "authors"."id")"}
  };

  query_context qc;
  size_t index{0};
  for (const auto &jd : data->joins) {
    REQUIRE(jd.join_table.name == expected_join_data[index].first);
    REQUIRE(jd.condition->evaluate(db.dialect(), qc) == expected_join_data[index].second);
    ++index;
  }

  REQUIRE(data->where_clause);
  auto cond = data->where_clause->evaluate(db.dialect(), qc);
  REQUIRE(cond == R"("books"."id" = 17)");

  auto q = db.query(scm)
    .select(data->columns)
    .from(data->root_table_name);

  for (auto &jd : data->joins) {
    q.join_left(jd.join_table)
      .on(std::move(jd.condition));
  }
  auto context = q
    .where(std::move(data->where_clause))
    .build();
}

TEST_CASE("Create sql query data for entity with eager has many belongs to", "[query][entity][builder]") {
  using namespace matador::test;
  connection db("noop://noop.db");
  schema scm("noop");
  scm.attach<product>("products");
  scm.attach<order_details>("order_details");
  scm.attach<order>("orders");

  entity_query_builder eqb(scm);

  auto data = eqb.build<order>(17);

  REQUIRE(data.is_ok());
  REQUIRE(data->root_table_name == "orders");
  REQUIRE(data->joins.size() == 1);
  const std::vector<column> expected_columns = {
    { "orders", "order_id", "C01" },
    { "orders", "order_date", "C02" },
    { "orders", "required_date", "C03" },
    { "orders", "shipped_date", "C04" },
    { "orders", "ship_via", "C05" },
    { "orders", "freight", "C06" },
    { "orders", "ship_name", "C07" },
    { "orders", "ship_address", "C08" },
    { "orders", "ship_city", "C09" },
    { "orders", "ship_region", "C10" },
    { "orders", "ship_postal_code", "C11" },
    { "orders", "ship_country", "C12" },
    { "order_details", "order_details_id", "C13" },
    { "order_details", "order_id", "C14" },
    { "order_details", "product_id", "C15" }
  };
  REQUIRE(data->columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(data->columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "order_details", R"("orders"."order_id" = "order_details"."order_id")"}
  };

  query_context qc;
  size_t index{0};
  for (const auto &jd : data->joins) {
    REQUIRE(jd.join_table.name == expected_join_data[index].first);
    REQUIRE(jd.condition->evaluate(db.dialect(), qc) == expected_join_data[index].second);
    ++index;
  }

  REQUIRE(data->where_clause);
  auto cond = data->where_clause->evaluate(db.dialect(), qc);
  REQUIRE(cond == R"("orders"."order_id" = 17)");
}

TEST_CASE("Create sql query data for entity with eager many to many", "[query][entity][builder]") {
  using namespace matador::test;
  connection db("noop://noop.db");
  schema scm("noop");
  scm.attach<recipe>("recipes");
  scm.attach<ingredient>("ingredients");
  scm.attach<recipe_ingredient>("recipe_ingredients");

  entity_query_builder eqb(scm);

  auto data = eqb.build<ingredient>(17);

  REQUIRE(data.is_ok());
  REQUIRE(data->root_table_name == "ingredients");
  REQUIRE(data->joins.size() == 2);
  const std::vector<column> expected_columns {
    { "ingredients", "id", "C01" },
    { "ingredients", "name", "C02" },
    { "recipes", "id", "C03" },
    { "recipes", "name", "C04" }
  };
  REQUIRE(data->columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(data->columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "recipe_ingredients", R"("ingredients"."id" = "recipe_ingredients"."ingredient_id")"},
    { "recipes", R"("recipe_ingredients"."recipe_id" = "recipes"."id")"}
  };

  query_context qc;
  size_t index{0};
  for (const auto &jd : data->joins) {
    REQUIRE(jd.join_table.name == expected_join_data[index].first);
    REQUIRE(jd.condition->evaluate(db.dialect(), qc) == expected_join_data[index].second);
    ++index;
  }

  REQUIRE(data->where_clause);
  auto cond = data->where_clause->evaluate(db.dialect(), qc);
  REQUIRE(cond == R"("ingredients"."id" = 17)");
}

TEST_CASE("Create sql query data for entity with eager many to many (inverse part)", "[query][entity][builder]") {
  using namespace matador::test;
  connection db("noop://noop.db");
  schema scm("noop");
  scm.attach<student>("students");
  scm.attach<course>("courses");
  scm.attach<student_course>("student_courses");

  entity_query_builder eqb(scm);

  auto data = eqb.build<course>(17);

  REQUIRE(data.is_ok());
  REQUIRE(data->root_table_name == "courses");
  REQUIRE(data->joins.size() == 2);
  const std::vector<column> expected_columns {
    { "courses", "id", "C01" },
    { "courses", "title", "C02" },
    { "students", "id", "C03" },
    { "students", "name", "C04" }
  };
  REQUIRE(data->columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(data->columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "student_courses", R"("courses"."id" = "student_courses"."course_id")"},
    { "students", R"("student_courses"."student_id" = "students"."id")"}
  };

  query_context qc;
  size_t index{0};
  for (const auto &jd : data->joins) {
    REQUIRE(jd.join_table.name == expected_join_data[index].first);
    REQUIRE(jd.condition->evaluate(db.dialect(), qc) == expected_join_data[index].second);
    ++index;
  }

  REQUIRE(data->where_clause);
  auto cond = data->where_clause->evaluate(db.dialect(), qc);
  REQUIRE(cond == R"("courses"."id" = 17)");

    const auto qry = db.query(scm)
      .select(data->columns)
      .from(data->root_table_name)
      .join_left(data->joins)
      .where(std::move(data->where_clause))
      .order_by({data->root_table_name, data->pk_column_})
      .asc()
      .build();
}