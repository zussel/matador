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
  REQUIRE(data->root_table->name == "flights");
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
    REQUIRE(jd.join_table->name == expected_join_data[index].first);
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
  REQUIRE(data->root_table->name == "books");
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
    REQUIRE(expected_columns[i].equals(data->columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "authors", R"("T01"."author_id" = C03)"}
  };

  query_context qc;
  size_t index{0};
  for (const auto &jd : data->joins) {
    REQUIRE(jd.join_table->name == expected_join_data[index].first);
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

  std::cout << sql.sql << "\n";
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
  REQUIRE(data->root_table->name == "orders");
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
    REQUIRE(jd.join_table->name == expected_join_data[index].first);
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
  REQUIRE(data->root_table->name == "ingredients");
  REQUIRE(data->joins.size() == 2);
  const table ingredients_table{"ingredients", "T01"};
  const table recipes_table{"recipes", "T03"};
  const std::vector<column> expected_columns {
    { ingredients_table, "id", "C01" },
    { ingredients_table, "name", "C02" },
    { recipes_table, "id", "C03" },
    { recipes_table, "name", "C04" }
  };
  REQUIRE(data->columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(data->columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "recipe_ingredients", R"(C01 = "T02"."ingredient_id")"},
    { "recipes", R"("T02"."recipe_id" = C03)"}
  };

  query_context qc;
  size_t index{0};
  for (const auto &jd : data->joins) {
    REQUIRE(jd.join_table->name == expected_join_data[index].first);
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

    std::cout << sql.sql << "\n";

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
  REQUIRE(data->root_table->name == "courses");
  REQUIRE(data->joins.size() == 2);
  const table courses_table{"courses", "T01"};
  const table students_table{"students", "T03"};
  const std::vector<column> expected_columns {
    { courses_table, "id", "C01" },
    { courses_table, "title", "C02" },
    { students_table, "id", "C03" },
    { students_table, "name", "C04" }
  };
  REQUIRE(data->columns.size() == expected_columns.size());
  for (size_t i = 0; i != expected_columns.size(); ++i) {
    REQUIRE(expected_columns[i].equals(data->columns[i]));
  }

  std::vector<std::pair<std::string, std::string>> expected_join_data {
    { "student_courses", R"(C01 = "T02"."course_id")"},
    { "students", R"("T02"."student_id" = C03)"}
  };

  query_context qc;
  size_t index{0};
  for (const auto &jd : data->joins) {
    REQUIRE(jd.join_table->name == expected_join_data[index].first);
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

    std::cout << sql.sql << "\n";
}