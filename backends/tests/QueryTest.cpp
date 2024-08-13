#include "catch2/catch_test_macros.hpp"

#include "matador/sql/column_definition.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/query_builder.hpp"

#include "QueryFixture.hpp"

#include "models/airplane.hpp"
#include "models/flight.hpp"
#include "models/person.hpp"
#include "models/recipe.hpp"

using namespace matador::sql;
using namespace matador::test;

TEST_CASE_METHOD(QueryFixture, "Create table with foreign key relation", "[query]")
{
  schema.attach<airplane>("airplane");
  schema.attach<flight>("flight");
  db.query(schema).create()
  .table<airplane>("airplane")
  .execute();

  REQUIRE(db.exists("airplane"));
  tables_to_drop.insert("airplane");

  db.query(schema).create()
  .table<flight>("flight")
  .execute();

  REQUIRE(db.exists("flight"));
  tables_to_drop.insert("flight");

  db.query(schema).drop().table("flight").execute();
  db.query(schema).drop().table("airplane").execute();

  REQUIRE(!db.exists("flight"));
  REQUIRE(!db.exists("airplane"));
}

TEST_CASE_METHOD(QueryFixture, "Execute select statement with where clause", "[query]")
{
  schema.attach<person>("person");
  db.query(schema).create()
  .table<person>("person")
  .execute();

  REQUIRE(db.exists("person"));
  tables_to_drop.insert("person");

  person george{7, "george", 45};
  george.image.push_back(37);

  auto res = db.query(schema)
  .insert()
  .into("person", column_generator::generate<person>(schema, true))
  .values(george)
  .execute();
  REQUIRE(res == 1);

  // fetch person as record
  auto result_record = db.query(schema)
    .select(column_generator::generate<person>(schema, true))
    .from("person")
    .where("id"_col == 7)
    .fetch_all();

  for (const auto &i: result_record) {
    REQUIRE(i.size() == 4);
    REQUIRE(i.at(0).name() == "id");
    REQUIRE(i.at(0).is_integer());
    REQUIRE(i.at(0).as<long long>() == george.id);
    REQUIRE(i.at(1).name() == "name");
    REQUIRE(i.at(1).is_varchar());
    REQUIRE(i.at(1).as<std::string>() == george.name);
    REQUIRE(i.at(2).name() == "age");
    REQUIRE(i.at(2).is_integer());
    REQUIRE(i.at(2).as<long long>() == george.age);
  }

  // fetch person as person
  auto result_person = db.query(schema)
    .select(column_generator::generate<person>(schema, true))
    .from("person")
    .where("id"_col == 7)
    .fetch_all<person>();

  for (const auto &i: result_person) {
    REQUIRE(i.id == 7);
    REQUIRE(i.name == "george");
    REQUIRE(i.age == 45);
  }

  db.query(schema).drop().table("person").execute();
}

TEST_CASE_METHOD(QueryFixture, "Execute insert statement", "[query]")
{
  db.query(schema).create()
  .table("person", {
  make_pk_column<unsigned long>("id"),
  make_column<std::string>("name", 255),
  make_column<std::string>("color", 63)
  })
  .execute();

  REQUIRE(db.exists("person"));
  tables_to_drop.insert("person");

  auto res = db.query(schema).insert()
  .into("person", {{"", "id", ""}, {"", "name", ""}, {"", "color", ""}})
  .values({7, "george", "green"})
  .execute();

  REQUIRE(res == 1);

  // fetch person as record
  auto result_record = db.query(schema).select({"id", "name", "color"})
  .from("person")
  .where("id"_col == 7)
  .fetch_all();

  for (const auto &i: result_record) {
    REQUIRE(i.size() == 3);
    REQUIRE(i.at(0).name() == "id");
    REQUIRE(i.at(0).is_integer());
    REQUIRE(i.at(0).as<unsigned long>() == 7);
    REQUIRE(i.at(1).name() == "name");
    REQUIRE(i.at(1).is_varchar());
    REQUIRE(i.at(1).as<std::string>() == "george");
    REQUIRE(i.at(2).name() == "color");
    REQUIRE(i.at(2).is_varchar());
    REQUIRE(i.at(2).as<std::string>() == "green");
  }

  db.query(schema).drop().table("person").execute();
}

TEST_CASE_METHOD(QueryFixture, "Select statement with foreign key", "[query]")
{
  schema.attach<airplane>("airplane");
  schema.attach<flight>("flight");
  db.query(schema).create()
  .table<airplane>("airplane")
  .execute();

  REQUIRE(db.exists("airplane"));
  tables_to_drop.insert("airplane");

  db.query(schema).create()
  .table<flight>("flight")
  .execute();

  REQUIRE(db.exists("flight"));
  tables_to_drop.insert("flight");

  std::vector<matador::object_ptr<airplane>> planes{
    matador::object_ptr<airplane>(new airplane{1, "Airbus", "A380"}),
    matador::object_ptr<airplane>(new airplane{2, "Boeing", "707"}),
    matador::object_ptr<airplane>(new airplane{3, "Boeing", "747"})
  };

  for (const auto &plane: planes) {
    auto res = db.query(schema)
      .insert()
      .into("airplane", column_generator::generate<airplane>(schema, true))
      .values(*plane)
      .execute();
    REQUIRE(res == 1);
  }

  auto count = db.query(schema)
    .select({count_all()})
    .from("airplane")
    .fetch_value<int>().value();
  REQUIRE(count == 3);

  flight f4711{4, planes.at(1), "hans"};

  auto res = db.query(schema)
    .insert()
    .into("flight", column_generator::generate<flight>(schema, true))
    .values(f4711)
    .execute();
  REQUIRE(res == 1);

  auto f = *db.query(schema)
    .select(column_generator::generate<flight>(schema, true))
    .from("flight")
    .fetch_all().begin();
  REQUIRE(f.at(0).as<unsigned long>() == 4);
  REQUIRE(f.at(1).as<unsigned long>() == 2);
  REQUIRE(f.at(2).as<std::string>() == "hans");

  db.query(schema).drop().table("flight").execute();
  db.query(schema).drop().table("airplane").execute();
}

TEST_CASE_METHOD(QueryFixture, "Select statement with foreign key and join_left", "[query][join_left]")
{
  schema.attach<airplane>("airplane");
  schema.attach<flight>("flight");
  db.query(schema).create()
  .table<airplane>("airplane")
  .execute();

  REQUIRE(db.exists("airplane"));
  tables_to_drop.insert("airplane");

  db.query(schema).create()
    .table<flight>("flight")
    .execute();

  REQUIRE(db.exists("flight"));
  tables_to_drop.insert("flight");

  std::vector<matador::object_ptr<airplane>> planes{
    matador::object_ptr<airplane>(new airplane{1, "Airbus", "A380"}),
    matador::object_ptr<airplane>(new airplane{2, "Boeing", "707"}),
    matador::object_ptr<airplane>(new airplane{3, "Boeing", "747"})
  };

  for (const auto &plane: planes) {
    auto res = db.query(schema)
      .insert()
      .into("airplane", column_generator::generate<airplane>(schema, true))
      .values(*plane)
      .execute();
    REQUIRE(res == 1);
  }

  auto count = db.query(schema)
    .select({count_all()})
    .from("airplane")
    .fetch_value<int>().value();
  REQUIRE(count == 3);

  std::vector<matador::object_ptr<flight>> flights{
    matador::object_ptr<flight>(new flight{4, planes.at(0), "hans"}),
    matador::object_ptr<flight>(new flight{5, planes.at(0), "otto"}),
    matador::object_ptr<flight>(new flight{6, planes.at(1), "george"}),
    matador::object_ptr<flight>(new flight{7, planes.at(2), "paul"})
  };

  for (const auto &f: flights) {
    auto res = db.query(schema)
      .insert()
      .into("flight", {"id", "airplane_id", "pilot_name"})
      .values(*f)
      .execute();
    REQUIRE(res == 1);
  }

  auto f = *db.query(schema)
    .select(column_generator::generate<flight>(schema, true))
    .from("flight")
    .fetch_all().begin();
  REQUIRE(f.at(0).as<unsigned long>() == 4);
  REQUIRE(f.at(1).as<unsigned long>() == 1);
  REQUIRE(f.at(2).as<std::string>() == "hans");

  auto result = db.query(schema).select({"f.id", "ap.brand", "ap.model", "f.pilot_name"})
    .from({"flight", "f"})
    .join_left({"airplane", "ap"})
    .on("f.airplane_id"_col == "ap.id"_col)
    .order_by("f.id").asc()
    .fetch_all();

  std::vector<std::pair<unsigned long, std::string>> expected_result {
    {4, "hans"},
    {5, "otto"},
    {6, "george"},
    {7, "paul"}
  };
  size_t index{0};
  for (const auto &r: result) {
    REQUIRE(r.size() == 4);
    REQUIRE(r.at(0).as<unsigned long>() == expected_result[index].first);
    REQUIRE(r.at(3).as<std::string>() == expected_result[index++].second);
  }

  db.query(schema).drop().table("flight").execute();
  db.query(schema).drop().table("airplane").execute();
}

TEST_CASE_METHOD(QueryFixture, "Select statement with foreign key and for single entity", "[query][join_left][find]") {
  schema.attach<airplane>("airplane");
  schema.attach<flight>("flight");
  db.query(schema).create()
  .table<airplane>("airplane")
  .execute();

  REQUIRE(db.exists("airplane"));
  tables_to_drop.insert("airplane");

  db.query(schema).create()
    .table<flight>("flight")
    .execute();

  REQUIRE(db.exists("flight"));
  tables_to_drop.insert("flight");

  std::vector<matador::object_ptr<airplane>> planes{
    matador::object_ptr<airplane>(new airplane{1, "Airbus", "A380"}),
    matador::object_ptr<airplane>(new airplane{2, "Boeing", "707"}),
    matador::object_ptr<airplane>(new airplane{3, "Boeing", "747"})
  };

  for (const auto &plane: planes) {
    auto res = db
      .query(schema)
      .insert()
      .into("airplane", column_generator::generate<airplane>(schema, true))
      .values(*plane)
      .execute();
    REQUIRE(res == 1);
  }

  auto count = db
    .query(schema)
    .select({count_all()})
    .from("airplane")
    .fetch_value<int>().value();
  REQUIRE(count == 3);

  std::vector<matador::object_ptr<flight>> flights{
    matador::object_ptr<flight>(new flight{4, planes.at(0), "hans"}),
    matador::object_ptr<flight>(new flight{5, planes.at(0), "otto"}),
    matador::object_ptr<flight>(new flight{6, planes.at(1), "george"}),
    matador::object_ptr<flight>(new flight{7, planes.at(2), "paul"})
  };

  for (const auto &f: flights) {
    auto res = db.query(schema)
      .insert()
      .into("flight", column_generator::generate<flight>(schema, true))
      .values(*f)
      .execute();
    REQUIRE(res == 1);
  }

  auto f = db.query(schema)
    .select(column_generator::generate<flight>(schema, true))
    .from("flight")
    .fetch_one();
  REQUIRE(f.has_value());
  REQUIRE(f->at(0).as<unsigned long>() == 4);
  REQUIRE(f->at(1).as<unsigned long>() == 1);
  REQUIRE(f->at(2).as<std::string>() == "hans");

  auto result = db
    .query(schema)
    .select({"f.id", "f.airplane_id", "ap.brand", "ap.model", "f.pilot_name"})
    .from({"flight", "f"})
    .join_left({"airplane", "ap"})
    .on("f.airplane_id"_col == "ap.id"_col)
    .where("f.id"_col == 4)
    .fetch_one<flight>();

  auto expected_flight = flights[0];

  REQUIRE(result);
  REQUIRE(result->id == expected_flight->id);
  REQUIRE(result->pilot_name == expected_flight->pilot_name);
  REQUIRE(result->airplane.get());
  REQUIRE(result->airplane->id == 1);
  REQUIRE(result->airplane->model == "A380");
  REQUIRE(result->airplane->brand == "Airbus");

  db.query(schema).drop().table("flight").execute();
  db.query(schema).drop().table("airplane").execute();
}

TEST_CASE_METHOD(QueryFixture, "Select statement with many to many relationship", "[query][join][many_to_many]") {
  schema.attach<recipe>("recipes");
  schema.attach<ingredient>("ingredients");
  schema.attach<recipe_ingredient>("recipe_ingredients");
  db.query(schema).create()
  .table<recipe>("recipes")
  .execute();

  REQUIRE(db.exists("recipes"));
  tables_to_drop.insert("recipes");

  db.query(schema).create()
  .table<ingredient>("ingredients")
  .execute();

  REQUIRE(db.exists("ingredients"));
  tables_to_drop.insert("ingredients");

  db.query(schema).create()
  .table<recipe_ingredient>("recipe_ingredients")
  .execute();

  REQUIRE(db.exists("recipe_ingredients"));
  tables_to_drop.insert("recipe_ingredients");

  std::vector<ingredient> ingredients {
    {1, "Apple"},
    {2, "Strawberry"},
    {3, "Pineapple"},
    {4, "Sugar"},
    {5, "Flour"},
    {6, "Butter"},
    {7, "Beans"}
  };

  for (const auto &i: ingredients) {
    auto res = db
    .query(schema)
    .insert()
    .into("ingredients", column_generator::generate<ingredient>(schema, true))
    .values(i)
    .execute();
    REQUIRE(res == 1);
  }

  std::vector<recipe> recipes{
    {7, "Apple Crumble"},
    {8, "Beans Chili"},
    {9, "Fruit Salad"}
  };

  for (const auto &r: recipes) {
    auto res = db
    .query(schema)
    .insert()
    .into("recipes", column_generator::generate<recipe>(schema, true))
    .values(r)
    .execute();
    REQUIRE(res == 1);
  }

  std::vector<std::pair<int, int>> recipe_ingredients {
    { 7, 1 },
    { 7, 4 },
    { 7, 5 },
    { 8, 6 },
    { 8, 7 },
    { 9, 1 },
    { 9, 2 },
    { 9, 3 }
  };

  for (const auto &ri: recipe_ingredients) {
    auto res = db
    .query(schema)
    .insert()
    .into("recipe_ingredients", column_generator::generate<recipe_ingredient>(schema, true))
    .values({ri.first, ri.second})
    .execute();
    REQUIRE(res == 1);
  }

  auto result = db
    .query(schema)
    .select({"r.id", "r.name", "ri.ingredient_id"})
    .from({"recipes", "r"})
    .join_left({"recipe_ingredients", "ri"})
    .on("r.id"_col == "ri.recipe_id"_col)
    .fetch_all();

  std::vector<std::tuple<unsigned long, std::string, unsigned long>> expected_result_one_join {
    {7, "Apple Crumble", 1},
    {7, "Apple Crumble", 4},
    {7, "Apple Crumble", 5},
    {8, "Beans Chili", 6},
    {8, "Beans Chili", 7},
    {9, "Fruit Salad", 1},
    {9, "Fruit Salad", 2},
    {9, "Fruit Salad", 3}
  };
  size_t index{0};
  for (const auto &r: result) {
    REQUIRE(r.size() == 3);
    REQUIRE(r.at(0).as<unsigned long>().value() == std::get<0>(expected_result_one_join[index]));
    REQUIRE(r.at(1).as<std::string>().value() == std::get<1>(expected_result_one_join[index]));
    REQUIRE(r.at(2).as<unsigned long>().value() == std::get<2>(expected_result_one_join[index]));
    ++index;
  }

  result = db
  .query(schema)
  .select({"r.id", "r.name", "ri.ingredient_id", "i.name"})
  .from({"recipes", "r"})
  .join_left({"recipe_ingredients", "ri"}).on("r.id"_col == "ri.recipe_id"_col)
  .join_left({"ingredients", "i"}).on("ri.ingredient_id"_col == "i.id"_col)
  .fetch_all();

  std::vector<std::tuple<unsigned long, std::string, unsigned long, std::string>> expected_result_two_joins {
    {7, "Apple Crumble", 1, "Apple"},
    {7, "Apple Crumble", 4, "Sugar"},
    {7, "Apple Crumble", 5, "Flour"},
    {8, "Beans Chili", 6, "Butter"},
    {8, "Beans Chili", 7, "Beans"},
    {9, "Fruit Salad", 1, "Apple"},
    {9, "Fruit Salad", 2, "Strawberry"},
    {9, "Fruit Salad", 3, "Pineapple"}
  };
  index = 0;
  for (const auto &r: result) {
    REQUIRE(r.size() == 4);
    REQUIRE(r.at(0).as<unsigned long>().value() == std::get<0>(expected_result_two_joins[index]));
    REQUIRE(r.at(1).as<std::string>().value() == std::get<1>(expected_result_two_joins[index]));
    REQUIRE(r.at(2).as<unsigned long>().value() == std::get<2>(expected_result_two_joins[index]));
    REQUIRE(r.at(3).as<std::string>().value() == std::get<3>(expected_result_two_joins[index]));
    ++index;
  }

  result = db
  .query(schema)
  .select({"r.id", "r.name", "ri.ingredient_id", "i.name"})
  .from({"recipes", "r"})
  .join_left({"recipe_ingredients", "ri"}).on("r.id"_col == "ri.recipe_id"_col)
  .join_left({"ingredients", "i"}).on("ri.ingredient_id"_col == "i.id"_col)
  .where("r.id"_col == 8)
  .fetch_all();

  index = 3;
  for (const auto &r: result) {
    REQUIRE(r.size() == 4);
    REQUIRE(r.at(0).as<unsigned long>().value() == std::get<0>(expected_result_two_joins[index]));
    REQUIRE(r.at(1).as<std::string>().value() == std::get<1>(expected_result_two_joins[index]));
    REQUIRE(r.at(2).as<unsigned long>().value() == std::get<2>(expected_result_two_joins[index]));
    REQUIRE(r.at(3).as<std::string>().value() == std::get<3>(expected_result_two_joins[index]));
    ++index;
  }

  db.query(schema).drop().table("recipe_ingredients").execute();
  db.query(schema).drop().table("recipes").execute();
  db.query(schema).drop().table("ingredients").execute();
}