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

    if (data.is_error()) {
        std::cout << static_cast<int>(data.err()) << std::endl;
    }
  REQUIRE(data.is_ok());
  REQUIRE(data->root_table.value().get().name == "flights");
  REQUIRE(data->joins.size() == 1);

  const table flights_table{"flights", "T01"};
  const table airplanes_table{"airplanes", "T02"};
  const std::vector<column> expected_columns {
    { flights_table, "id", "C01" },
    { airplanes_table, "id", "C02" },
    { airplanes_table, "brand", "C03" },
    { airplanes_table, "model", "C04" },
    { flights_table, "pilot_name", "C05" },
  };
  REQUIRE(data->columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(data->columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "airplanes", R"("T01"."airplane_id" = C02)"}
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
  REQUIRE(cond == R"(C01 = 17)");

  const auto sql = db.query(scm)
    .select(data->columns)
    .from(*data->root_table)
    .join_left(data->joins)
    .where(std::move(data->where_clause))
    .order_by(column{*data->pk_column_})
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
  REQUIRE(data->root_table->get().name == "books");
  REQUIRE(data->joins.size() == 1);
  const table books_table{"books", "T01"};
  const table authors_table{"authors", "T02"};
  const std::vector<column> expected_columns {
    { books_table, "id", "C01" },
    { books_table, "title", "C02" },
    { authors_table, "id", "C03" },
    { authors_table, "first_name", "C04" },
    { authors_table, "last_name", "C05" },
    { authors_table, "date_of_birth", "C06" },
    { authors_table, "year_of_birth", "C07" },
    { authors_table, "distinguished", "C08" },
    { books_table, "published_in", "C09" }
  };
  REQUIRE(data->columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(expected_columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "authors", R"("T01"."author_id" = C03)"}
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
  REQUIRE(cond == R"(C01 = 17)");

  auto q = db.query(scm)
    .select(data->columns)
    .from(data->root_table->get().name);

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
  REQUIRE(data->root_table->get().name == "orders");
  REQUIRE(data->joins.size() == 1);
  const table orders_table{"orders", "T01"};
  const table order_details_table{"order_details", "T02"};
  const std::vector<column> expected_columns = {
    { orders_table, "order_id", "C01" },
    { orders_table, "order_date", "C02" },
    { orders_table, "required_date", "C03" },
    { orders_table, "shipped_date", "C04" },
    { orders_table, "ship_via", "C05" },
    { orders_table, "freight", "C06" },
    { orders_table, "ship_name", "C07" },
    { orders_table, "ship_address", "C08" },
    { orders_table, "ship_city", "C09" },
    { orders_table, "ship_region", "C10" },
    { orders_table, "ship_postal_code", "C11" },
    { orders_table, "ship_country", "C12" },
    { order_details_table, "order_details_id", "C13" },
    { order_details_table, "order_id", "C14" },
    { order_details_table, "product_id", "C15" }
  };
  REQUIRE(data->columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(data->columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "order_details", R"(C01 = C14)"}
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
  REQUIRE(cond == R"(C01 = 17)");
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
  REQUIRE(data->root_table->get().name == "ingredients");
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
  REQUIRE(data->root_table->get().name == "courses");
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
      .from(*data->root_table)
      .join_left(data->joins)
      .where(std::move(data->where_clause))
      .order_by(column{*data->pk_column_})
      .asc()
      .build();
}