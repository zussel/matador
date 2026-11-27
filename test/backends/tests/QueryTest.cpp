#include "catch2/catch_test_macros.hpp"

#include "matador/query/builder/builder.hpp"
#include "matador/query/criteria.hpp"
#include "matador/query/mapping/generators.hpp"
#include "matador/query/mapping/table_pk_generator.hpp"
#include "matador/query/builder/query.hpp"
#include "matador/query/meta_table_macro.hpp"

#include "fixtures/QueryFixture.hpp"

#include "models/airplane.hpp"
#include "models/author.hpp"
#include "models/book.hpp"
#include "models/department.hpp"
#include "models/flight.hpp"
#include "models/person.hpp"
#include "models/recipe.hpp"
#include "models/shipment.hpp"
#include "models/model_metas.hpp"

#include "../../query/utils/RecordPrinter.hpp"

using namespace matador;
using namespace matador::query;
using namespace matador::test;
using namespace matador::utils;
using namespace matador::query::meta;

static void initialize_resolver(const basic_schema& repo, connection& db) {
  resolver_producer_registry relation_producers;
  for (const auto &pair : repo.resolver_producers()) {
    pair.second->create_relation_producers(relation_producers);

    auto res = pair.second->build_query(db.dialect()).and_then([&db](query_context&& query_ctx) -> matador::result<statement, error> {
      query_ctx.resolver = db.resolver();
      return db.prepare(query_ctx);
    }).and_then([&pair, &db](statement&& stmt) -> matador::result<void, error> {
      db.resolver()->register_object_resolver(pair.second->produce(std::move(stmt)));

      return matador::ok<void>();
    }).or_else([](const auto &err) {
      return matador::failure<error>(err);
    });

    if (!res) {
      throw std::runtime_error(res.err().message());
    }
  }

  for (const auto &pair : relation_producers.joined_object_producers()) {
    auto res = pair.second->build_query(db.dialect())
                   .and_then([&db](query_context &&query_ctx) -> result<statement, error> {
                     query_ctx.resolver = db.resolver();
                     return db.prepare(query_ctx);
                   })
                   .and_then([&pair, &db](statement &&stmt) -> result<void, error> {
                     db.resolver()->register_joined_object_resolver(
                         pair.second->produce(std::move(stmt)), pair.second->root_type(),
                         pair.second->collection_name());

                     return ok<void>();
                   })
                   .or_else([](const auto &err) { return failure(err); });

    if (!res) {
      throw std::runtime_error(res.err().message());
    }
  }

  for (const auto &pair : relation_producers.collection_producers()) {
    auto res = pair.second->build_query(db.dialect())
                   .and_then([&db](query_context &&query_ctx) -> result<statement, error> {
                     query_ctx.resolver = db.resolver();
                     return db.prepare(query_ctx);
                   })
                   .and_then([&pair, &db](statement &&stmt) -> result<void, error> {
                     db.resolver()->register_collection_resolver(
                         pair.second->produce(std::move(stmt), *db.resolver()));

                     return ok<void>();
                   })
                   .or_else([](const auto &err) { return failure(err); });

    if (!res) {
      throw std::runtime_error(res.err().message());
    }
  }
}

TEST_CASE_METHOD(QueryFixture, "Create table with foreign key relation", "[query][foreign][relation]") {
  const auto result = repo.attach<airplane>("airplane")
    .and_then([this] { return repo.attach<flight>("flight");})
    .and_then([this] {return manager.create(db); });
  REQUIRE(result.is_ok());

  check_table_exists("airplane");
  check_table_exists("flight");
}

TEST_CASE_METHOD(QueryFixture, "Execute select statement with where clause", "[query][where]") {
  REQUIRE(repo.attach<person>("persons"));
  auto result = manager.create(db);
  REQUIRE(result.is_ok());

  check_table_exists(PERSON.table_name());

  person george{7, "george", 45};
  george.image.emplace_back(37);

  auto res = insert()
    .into(PERSON, {PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .values(george)
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  // fetch person as record
  auto result_record = select({PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .from(PERSON)
    .where(PERSON.id == 7)
    .fetch_all(db);
  REQUIRE(result_record.is_ok());

  for (const auto &i: *result_record) {
    REQUIRE(i.size() == 4);
    REQUIRE(i.at(0).name() == "persons.id");
    REQUIRE(i.at(0).is_integer());
    REQUIRE(i.at(0).as<uint32_t>() == george.id);
    REQUIRE(i.at(1).name() == "persons.name");
    REQUIRE(i.at(1).is_varchar());
    REQUIRE(i.at(1).as<std::string>() == george.name);
    REQUIRE(i.at(2).name() == "persons.age");
    REQUIRE(i.at(2).is_integer());
    REQUIRE(i.at(2).as<uint32_t>() == george.age);
  }

  // fetch person as person
  auto result_person = select({PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .from(PERSON)
    .where(PERSON.id == 7)
    .fetch_all<person>(db);
  REQUIRE(result_person.is_ok());

  for (const auto &i: *result_person) {
    REQUIRE(i->id == 7);
    REQUIRE(i->name == "george");
    REQUIRE(i->age == 45);
  }
}

TEST_CASE_METHOD(QueryFixture, "Execute insert with returning", "[query][insert][returning]") {
  REQUIRE(repo.attach<person>("persons"));
  auto result = manager.create(db);
  REQUIRE(result.is_ok());

  check_table_exists(PERSON.table_name());

  person george{7, "george", 45};
  george.image.emplace_back(37);

  auto res = insert()
    .into(PERSON, {PERSON.id, PERSON.name, PERSON.age, PERSON.image})
    .values(george)
    .returning(PERSON.id)
    .fetch_one(db);
  REQUIRE(res.is_ok());

  auto rec = res.release();
  REQUIRE(rec->size() == 1);
  REQUIRE(rec->at(0).name() == "persons.id");
  REQUIRE(rec->at(0).is_integer());
  REQUIRE(rec->at(0).as<uint32_t>() == 7);
}

TEST_CASE_METHOD(QueryFixture, "Test sequence", "[query][sequence]") {
  auto result = create()
    .sequence("person_seq")
    .execute(db);
  REQUIRE(result.is_ok());

  auto next_id = select()
    .nextval("person_seq")
    .fetch_value<uint32_t>(db);

  REQUIRE(next_id.is_ok());
  REQUIRE(*next_id == 1);

  result = drop()
    .sequence("person_seq")
    .execute(db);
  REQUIRE(result.is_ok());
}

TEST_CASE_METHOD(QueryFixture, "Test table primary key generator", "[query][table-pk-generator]") {
  auto created = create().table("test_sequence_table").columns({
    col("name", basic_type::Varchar, 255).not_null().primary_key(),
    col("next_id", basic_type::Int64)
  }).execute(db);
  REQUIRE(created.is_ok());
  tables_to_drop.emplace("test_sequence_table");
  REQUIRE(insert().into("test_sequence_table", {"name", "next_id"}).values({"books", 1}).execute(db).is_ok());

  table_pk_generator generator("test_sequence_table", "books");
  auto first = generator.next_id(db);
  REQUIRE(first.is_ok());
  REQUIRE(*first == 1);
  auto second = generator.next_id(db);
  REQUIRE(second.is_ok());
  REQUIRE(*second == 2);
  auto current = generator.current_id(db);
  REQUIRE(current.is_ok());
  REQUIRE(*current == 3);
  table_pk_generator missing("test_sequence_table", "missing");
  REQUIRE_FALSE(missing.next_id(db).is_ok());
}

TEST_CASE_METHOD(QueryFixture, "Execute insert statement", "[query][insert]") {
  auto res = create()
    .table("person")
    .columns({
        col("id", basic_type::UInt32),
        col("name", basic_type::Varchar, 255),
        col("color", basic_type::Varchar, 63)
    })
    .constraints({
        constraint("PK_person").primary_key("id")
    })
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 0);

  REQUIRE(db.exists("person"));
  tables_to_drop.emplace("person");

  res = insert()
    .into("person", {"id", "name", "color"})
    .values({7, "george", "green"})
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  // fetch person as record
  auto result_record = select({"id", "name", "color"})
    .from("person")
    .where("id"_col == 7)
    .fetch_all(db);
  REQUIRE(result_record.is_ok());

  for (const auto &i: *result_record) {
    REQUIRE(i.size() == 3);
    REQUIRE(i.at(0).name() == "id");
    REQUIRE(i.at(0).is_integer());
    REQUIRE(i.at(0).as<uint32_t>() == 7);
    REQUIRE(i.at(1).name() == "name");
    REQUIRE(i.at(1).is_varchar());
    REQUIRE(i.at(1).as<std::string>() == "george");
    REQUIRE(i.at(2).name() == "color");
    REQUIRE(i.at(2).is_varchar());
    REQUIRE(i.at(2).as<std::string>() == "green");
  }
}

TEST_CASE_METHOD(QueryFixture, "Select statement with foreign key", "[query][foreign]") {
  auto result = repo.attach<airplane>("airplanes")
    .and_then( [this] { return repo.attach<flight>("flights");} )
    .and_then([this] {return manager.create(db); });
  REQUIRE(result.is_ok());

  REQUIRE(db.exists(AIRPLANE.table_name()));
  REQUIRE(db.exists(FLIGHT.table_name()));

  std::vector planes{
    make_object<airplane>(1, "Airbus", "A380"),
    make_object<airplane>(2, "Boeing", "707"),
    make_object<airplane>(3, "Boeing", "747")
  };

  for (const auto &plane: planes) {
    auto res = insert()
      .into(AIRPLANE, {AIRPLANE.id, AIRPLANE.brand, AIRPLANE.model})
      .values(plane)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto count = select({count_all()})
    .from(AIRPLANE)
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(*count == 3);


  flight f4711{4, planes.at(1), "hans"};

  auto res = insert()
    .into(FLIGHT, {FLIGHT.id, FLIGHT.airplane_id, FLIGHT.pilot_name})
    .values(f4711)
    .execute(db);
  REQUIRE(res.is_ok());
  REQUIRE(res->affected_rows == 1);

  auto f = select({FLIGHT.id, FLIGHT.airplane_id, FLIGHT.pilot_name})
    .from(FLIGHT)
    .fetch_one(db);
  REQUIRE(f.is_ok());

  REQUIRE(f.value()->at(0).as<uint32_t>() == 4);
  REQUIRE(f.value()->at(1).as<uint32_t>() == 2);
  REQUIRE(f.value()->at(2).as<std::string>() == "hans");
}

TEST_CASE_METHOD(QueryFixture, "Select statement with foreign key and join_left", "[query][foreign][join_left]") {
  auto result = repo.attach<airplane>("airplanes")
    .and_then( [this] { return repo.attach<flight>("flights");} )
    .and_then([this] {return manager.create(db); });
  REQUIRE(result.is_ok());

  REQUIRE(db.exists(AIRPLANE.table_name()));
  REQUIRE(db.exists(FLIGHT.table_name()));

  std::vector planes{
    make_object<airplane>(1, "Airbus", "A380"),
    make_object<airplane>(2, "Boeing", "707"),
    make_object<airplane>(3, "Boeing", "747")
  };

  for (const auto &plane: planes) {
    auto res = insert()
      .into(AIRPLANE, {AIRPLANE.id, AIRPLANE.brand, AIRPLANE.model})
      .values(plane)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto count = select({count_all()})
    .from(AIRPLANE)
    .fetch_value<int>(db).value();
  REQUIRE(count == 3);

  std::vector flights{
    make_object<flight>(4, planes.at(0), "hans"),
    make_object<flight>(5, planes.at(0), "otto"),
    make_object<flight>(6, planes.at(1), "george"),
    make_object<flight>(7, planes.at(2), "paul"),
  };

  for (const auto &f: flights) {
    auto res = insert()
      .into(FLIGHT, {FLIGHT.id, FLIGHT.airplane_id, FLIGHT.pilot_name})
      .values(f)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto flight_result = select({FLIGHT.id, FLIGHT.airplane_id, FLIGHT.pilot_name})
    .from(FLIGHT)
    .fetch_one(db);
  REQUIRE(flight_result.is_ok());
  REQUIRE(flight_result->has_value());
  REQUIRE(flight_result.value()->at(0).as<uint32_t>() == 4);
  REQUIRE(flight_result.value()->at(1).as<uint32_t>() == 1);
  REQUIRE(flight_result.value()->at(2).as<std::string>() == "hans");

  const auto f = FLIGHT.as("f");
  const auto ap = AIRPLANE.as("ap");

  auto select_result = select({f.id, ap.brand, ap.model, f.pilot_name})
    .from(f)
    .join_left(ap)
    .on(f.airplane_id == ap.id)
    .order_by(f.id).asc()
    .fetch_all(db);
  REQUIRE(select_result.is_ok());

  std::vector<std::pair<uint32_t, std::string>> expected_result {
    {4, "hans"},
    {5, "otto"},
    {6, "george"},
    {7, "paul"}
  };
  size_t index{0};
  for (const auto &r: *select_result) {
    REQUIRE(r.size() == 4);
    REQUIRE(r.at(0).as<uint32_t>() == expected_result[index].first);
    REQUIRE(r.at(3).as<std::string>() == expected_result[index++].second);
  }
}

TEST_CASE_METHOD(QueryFixture, "Select statement with foreign key and for single entity", "[query][join_left][find]") {
  auto result = repo.attach<airplane>("airplanes")
    .and_then([this] { return repo.attach<flight>("flights");})
    .and_then([this] {return manager.create(db); });
  REQUIRE(result.is_ok());

  initialize_resolver(repo, db);

  REQUIRE(db.exists(AIRPLANE.table_name()));
  REQUIRE(db.exists(FLIGHT.table_name()));

  std::vector planes{
    make_object<airplane>(1, "Airbus", "A380"),
    make_object<airplane>(2, "Boeing", "707"),
    make_object<airplane>(3, "Boeing", "747")
  };

  for (const auto &plane: planes) {
    auto res = insert()
      .into(AIRPLANE, {AIRPLANE.id, AIRPLANE.brand, AIRPLANE.model})
      .values(plane)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto count = select({count_all()})
    .from(AIRPLANE)
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(count->has_value());
  REQUIRE(count->value() == 3);

  std::vector flights{
    make_object<flight>(4, planes.at(0), "hans"),
    make_object<flight>(5, planes.at(0), "otto"),
    make_object<flight>(6, planes.at(1), "george"),
    make_object<flight>(7, planes.at(2), "paul")
  };

  for (const auto &f: flights) {
    auto res = insert()
      .into(FLIGHT, {FLIGHT.id, FLIGHT.airplane_id, FLIGHT.pilot_name})
      .values(f)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto flight_result = select({FLIGHT.id, FLIGHT.airplane_id, FLIGHT.pilot_name})
    .from(FLIGHT)
    .fetch_one(db);
  REQUIRE(flight_result.is_ok());
  REQUIRE(flight_result->has_value());
  REQUIRE(flight_result.value()->at(0).as<uint32_t>() == 4);
  REQUIRE(flight_result.value()->at(1).as<uint32_t>() == 1);
  REQUIRE(flight_result.value()->at(2).as<std::string>() == "hans");

  const auto f = FLIGHT.as("f");
  const auto ap = AIRPLANE.as("ap");

  auto select_result = select({f.id, f.airplane_id, ap.brand, ap.model, f.pilot_name})
    .from(f)
    .join_left(ap)
    .on(f.airplane_id == ap.id)
    .where(f.id == 4)
    .fetch_one<flight>(db);

  auto expected_flight = flights[0];

  REQUIRE(select_result.is_ok());
  REQUIRE(*select_result);
  auto f1 = select_result.release();
  REQUIRE(f1->id == expected_flight->id);
  REQUIRE(f1->pilot_name == expected_flight->pilot_name);
  REQUIRE(f1->plane.get());
  REQUIRE(f1->plane->id == 1);
  REQUIRE(f1->plane->model == "A380");
  REQUIRE(f1->plane->brand == "Airbus");
}

TEST_CASE_METHOD(QueryFixture, "Select statement with many to many relationship", "[query][join][many_to_many]") {
  auto result = repo.attach<ingredient>("ingredients")
    .and_then( [this] { return repo.attach<recipe>("recipes"); } )
    .and_then([this] {return manager.create(db); });

  REQUIRE(db.exists(RECIPE.table_name()));
  REQUIRE(db.exists(INGREDIENT.table_name()));
  REQUIRE(db.exists(RECIPE_INGREDIENT.table_name()));

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
    auto res = insert()
    .into(INGREDIENT, {INGREDIENT.id, INGREDIENT.name})
    .values(i)
    .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  std::vector<recipe> recipes{
    {7, "Apple Crumble"},
    {8, "Beans Chili"},
    {9, "Fruit Salad"}
  };

  for (const auto &r: recipes) {
    auto res = insert()
    .into(RECIPE, {RECIPE.id, RECIPE.name})
    .values(r)
    .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
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

  for (const auto & [recipe_id, ingredient_id]: recipe_ingredients) {
    auto res = insert()
    .into(RECIPE_INGREDIENT, {RECIPE_INGREDIENT.recipe_id, RECIPE_INGREDIENT.ingredient_id})
    .values({recipe_id, ingredient_id})
    .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }


  const auto r = RECIPE.as("r");
  const auto ri= RECIPE_INGREDIENT.as("ri");
  const auto i = INGREDIENT.as("i");

  auto select_result = select({ r.id, r.name, ri.ingredient_id})
    .from(r)
    .join_left(ri)
    .on(r.id == ri.recipe_id)
    .fetch_all(db);
  REQUIRE(select_result.is_ok());

  std::vector<std::tuple<uint32_t, std::string, uint32_t>> expected_result_one_join {
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
  for (const auto &row: *select_result) {
    REQUIRE(row.size() == 3);
    REQUIRE(row.at(0).as<uint32_t>().value() == std::get<0>(expected_result_one_join[index]));
    REQUIRE(row.at(1).as<std::string>().value() == std::get<1>(expected_result_one_join[index]));
    REQUIRE(row.at(2).as<uint32_t>().value() == std::get<2>(expected_result_one_join[index]));
    ++index;
  }

  select_result = select({r.id, r.name, ri.ingredient_id, i.name})
    .from(r)
    .join_left(ri).on(r.id == ri.recipe_id)
    .join_left(i).on(ri.ingredient_id == i.id)
    .fetch_all(db);
  REQUIRE(result.is_ok());

  std::vector<std::tuple<uint32_t, std::string, uint32_t, std::string>> expected_result_two_joins {
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
  for (const auto &row: *select_result) {
    REQUIRE(row.size() == 4);
    REQUIRE(row.at(0).as<uint32_t>().value() == std::get<0>(expected_result_two_joins[index]));
    REQUIRE(row.at(1).as<std::string>().value() == std::get<1>(expected_result_two_joins[index]));
    REQUIRE(row.at(2).as<uint32_t>().value() == std::get<2>(expected_result_two_joins[index]));
    REQUIRE(row.at(3).as<std::string>().value() == std::get<3>(expected_result_two_joins[index]));
    ++index;
  }

  select_result = select({r.id, r.name, ri.ingredient_id, i.name})
    .from(r)
    .join_left(ri).on(r.id == ri.recipe_id)
    .join_left(i).on(ri.ingredient_id == i.id)
    .where(r.id == 8)
    .fetch_all(db);
  REQUIRE(result.is_ok());

  index = 3;
  for (const auto &row: *select_result) {
    REQUIRE(row.size() == 4);
    REQUIRE(row.at(0).as<uint32_t>().value() == std::get<0>(expected_result_two_joins[index]));
    REQUIRE(row.at(1).as<std::string>().value() == std::get<1>(expected_result_two_joins[index]));
    REQUIRE(row.at(2).as<uint32_t>().value() == std::get<2>(expected_result_two_joins[index]));
    REQUIRE(row.at(3).as<std::string>().value() == std::get<3>(expected_result_two_joins[index]));
    ++index;
  }
}

void print(std::ostream& out, const record& row) {
  for (const auto& f : row) {
    out << f << " ";
  }
}

TEST_CASE_METHOD(QueryFixture, "Test load entity with eager has many relation", "[query][has_many][eager]") {
  auto result = repo.attach<package>("packages")
    .and_then([this] { return repo.attach<shipment>("shipments"); })
    .and_then([this] {return manager.create(db); });
  REQUIRE(result.is_ok());

  initialize_resolver(repo, db);

  const std::vector shipments {
    make_object<shipment>(1, "4711"),
    make_object<shipment>(2, "0815")
  };

  for (const auto &sh: shipments) {
    auto res = insert()
      .into(SHIPMENT, SHIPMENT)
      .values(sh)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto count = select({count_all()})
    .from(SHIPMENT)
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(*count == 2);

  std::vector packages {
    make_object<package>(3, 15.4, shipments.at(0)),
    make_object<package>(4, 1.3, shipments.at(0)),
    make_object<package>(5, 30.9, shipments.at(1)),
    make_object<package>(6, 22.8, shipments.at(1)),
    make_object<package>(7, 17.2, shipments.at(1))
  };

  for (const auto &pkg: packages) {
    auto res = insert()
      .into(PACKAGE, PACKAGE)
      .values(pkg)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  count = select({count_all()})
    .from(PACKAGE)
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(*count == 5);

  auto pkgs = select({PACKAGE.id, PACKAGE.weight, PACKAGE.shipment_id, SHIPMENT.tracking_number})
    .from(PACKAGE)
    .join_left(SHIPMENT)
    .on( PACKAGE.shipment_id == SHIPMENT.id )
    .where( PACKAGE.weight > 20.0 && SHIPMENT.tracking_number == "0815" )
    .group_by({PACKAGE.id, SHIPMENT.tracking_number})
    .order_by(PACKAGE.weight).asc()
    .limit( 5 )
    .offset( 0 )
    .fetch_all(db);

  REQUIRE(pkgs.is_ok());

  RecordPrinter printer(std::cout);
  printer.print(*pkgs);

  auto shipment_records = select({SHIPMENT.tracking_number, SHIPMENT.id, PACKAGE.weight, PACKAGE.weight})
    .from(SHIPMENT)
    .join_left(PACKAGE)
    .on( SHIPMENT.id == PACKAGE.shipment_id )
    .fetch_all(db);

  REQUIRE(shipment_records.is_ok());

  printer.print(*shipment_records);

  auto shipment_result = select({SHIPMENT.id, SHIPMENT.tracking_number, PACKAGE.id, PACKAGE.weight, PACKAGE.shipment_id})
    .from(SHIPMENT)
    .join_left(PACKAGE)
    .on( SHIPMENT.id == PACKAGE.shipment_id )
    .order_by(SHIPMENT.id).asc()
    .fetch_all<shipment>(db);

  REQUIRE(shipment_result.is_ok());

  size_t index{0};
  std::vector<size_t> packages_sizes{2, 3};
  std::cout << "\n";
  for (const auto &s : *shipment_result) {
    REQUIRE(s->id == shipments.at(index)->id);
    REQUIRE(s->tracking_number == shipments.at(index)->tracking_number);
    REQUIRE(s->packages.size() == packages_sizes.at(index++));
  }
}

TEST_CASE_METHOD(QueryFixture, "Test load entity with lazy has many relation", "[query][has_many][lazy]") {
  auto result = repo.attach<author>("authors")
  .and_then( [this] { return repo.attach<book>("books"); } )
  .and_then([this] {
    return manager.create(db);
  } );
  REQUIRE(result.is_ok());

  initialize_resolver(repo, db);

  const std::vector authors {
    make_object<author>(1, "Michael", "Crichton", "23.10.1942", 1975, true),
    make_object<author>(2, "Steven", "King", "21.9.1947", 1956, false)
  };

  const std::vector books {
    make_object<book>(3, "Jurassic Park", authors.at(0), 1990),
    make_object<book>(4, "Timeline", authors.at(0), 1999),
    make_object<book>(5, "The Andromeda Strain", authors.at(0), 1969),
    make_object<book>(6, "Congo", authors.at(0), 1980),
    make_object<book>(7, "Prey", authors.at(0), 2002),
    make_object<book>(8, "Carrie", authors.at(1), 1974),
    make_object<book>(9, "The Shining", authors.at(1), 1977),
    make_object<book>(10, "It", authors.at(1), 1986),
    make_object<book>(11, "Misery", authors.at(1), 1987),
    make_object<book>(12, "The Dark Tower: The Gunslinger", authors.at(1), 1982)
  };

  for (const auto &a: authors) {
    auto res = insert()
      .into(AUTHOR, {AUTHOR.id, AUTHOR.first_name, AUTHOR.last_name, AUTHOR.date_of_birth, AUTHOR.year_of_birth, AUTHOR.distinguished})
      .values(a)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto count = select({count_all()})
    .from(AUTHOR)
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(*count == 2);

  for (const auto &b: books) {
    auto res = insert()
      .into(BOOK, {BOOK.id, BOOK.title, BOOK.author_id, BOOK.published_in})
      .values(b)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  count = select({count_all()})
    .from(BOOK)
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(*count == 10);

  auto author_result = select({AUTHOR.id, AUTHOR.first_name, AUTHOR.last_name, AUTHOR.date_of_birth, AUTHOR.year_of_birth, AUTHOR.distinguished})
    .from(AUTHOR)
    .order_by(AUTHOR.id).asc()
    .fetch_all<author>(db);

  REQUIRE(author_result.is_ok());
  size_t index{0};
  for (const auto &a : *author_result) {
    REQUIRE(a->id == authors.at(index)->id);
    REQUIRE(a->first_name == authors.at(index)->first_name);
    REQUIRE(a->last_name == authors.at(index)->last_name);
    REQUIRE(a->date_of_birth == authors.at(index)->date_of_birth);
    REQUIRE(a->year_of_birth == authors.at(index)->year_of_birth);
    REQUIRE(a->distinguished == authors.at(index)->distinguished);
    REQUIRE(!a->books.empty());
    for (const auto& b : a->books) {
      const auto title = b->title;
      // REQUIRE(b->id == books.at(index)->id);
      // REQUIRE(b->title == books.at(index)->title);
      // REQUIRE(b->author_id == books.at(index)->author_id);
      // REQUIRE(b->published_in == books.at(index)->published_in);
    }
    ++index;
  }
}

TEST_CASE_METHOD(QueryFixture, "Test load entity with lazy belongs to relation", "[query][belongs_to][lazy]") {
  auto result = repo.attach<department>("departments")
    .and_then([this] { return repo.attach<employee>("employees"); })
    .and_then([this] {return manager.create(db); });
  REQUIRE(result.is_ok());

  initialize_resolver(repo, db);

  const std::vector deps {
    make_object<department>(1, "Human Resources"),
    make_object<department>(2, "Invoices")
  };

  const std::vector emps {
    make_object<employee>(3, "Hans", "Wurst", deps.at(0)),
    make_object<employee>(4, "Steven", "Spielberg", deps.at(0)),
    make_object<employee>(5, "Julia", "Roberts", deps.at(0)),
    make_object<employee>(6, "Otto", "Walkes", deps.at(1)),
    make_object<employee>(7, "Miss", "Marple", deps.at(1))
  };

  // deps[0]->manager = emps[0];
  // deps[1]->manager = emps[4];

  for (const auto &dep: deps) {
    auto res = insert()
      .into(DEPARTMENT, {DEPARTMENT.id, DEPARTMENT.name})
      .values(dep)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto count = select({count_all()})
    .from(DEPARTMENT)
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(*count == 2);

  for (const auto &emp: emps) {
    auto res = insert()
      .into(EMPLOYEE, {EMPLOYEE.id, EMPLOYEE.first_name, EMPLOYEE.last_name, EMPLOYEE.dep_id})
      .values(emp)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  count = select({count_all()})
    .from(EMPLOYEE)
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(*count == 5);

  auto emps_result = select({EMPLOYEE.id, EMPLOYEE.first_name, EMPLOYEE.last_name, EMPLOYEE.dep_id})
    .from(EMPLOYEE)
    .order_by(EMPLOYEE.id).asc()
    .fetch_all<employee>(db);

  REQUIRE(emps_result.is_ok());
  size_t index{0};
  for (const auto &e : *emps_result) {
    REQUIRE(e->id == emps.at(index)->id);
    REQUIRE(e->first_name == emps.at(index)->first_name);
    REQUIRE(e->last_name == emps.at(index)->last_name);
    REQUIRE(e->dep->id == emps.at(index)->dep->id);
    REQUIRE(e->dep->name == emps.at(index)->dep->name);
    ++index;
  }
}

TEST_CASE_METHOD(QueryFixture, "Test load entity with eager belongs to relation", "[query][belongs_to][eager]") {
  auto result = repo.attach<book>("books")
    .and_then([this] { return repo.attach<author>("authors"); })
    .and_then([this] { return manager.create(db); });
  REQUIRE(result.is_ok());

  const std::vector authors {
    make_object<author>(1, "Michael", "Crichton", "23.10.1942", 1975, true),
    make_object<author>(2, "Steven", "King", "21.9.1947", 1956, false)
  };

  const std::vector books {
    make_object<book>(3, "Jurassic Park", authors.at(0), 1990),
    make_object<book>(4, "Timeline", authors.at(0), 1999),
    make_object<book>(5, "The Andromeda Strain", authors.at(0), 1969),
    make_object<book>(6, "Congo", authors.at(0), 1980),
    make_object<book>(7, "Prey", authors.at(0), 2002),
    make_object<book>(8, "Carrie", authors.at(1), 1974),
    make_object<book>(9, "The Shining", authors.at(1), 1977),
    make_object<book>(10, "It", authors.at(1), 1986),
    make_object<book>(11, "Misery", authors.at(1), 1987),
    make_object<book>(12, "The Dark Tower: The Gunslinger", authors.at(1), 1982)
  };

  for (const auto &a: authors) {
    auto res = insert()
      .into(AUTHOR, {AUTHOR.id, AUTHOR.first_name, AUTHOR.last_name, AUTHOR.date_of_birth, AUTHOR.year_of_birth, AUTHOR.distinguished})
      .values(a)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  auto count = select({count_all()})
    .from(AUTHOR)
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(*count == 2);

  for (const auto &b: books) {
    auto res = insert()
      .into(BOOK, {BOOK.id, BOOK.title, BOOK.author_id, BOOK.published_in})
      .values(b)
      .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  count = select({count_all()})
    .from(BOOK)
    .fetch_value<int>(db);
  REQUIRE(count.is_ok());
  REQUIRE(*count == 10);

  auto books_result = select({BOOK.id, BOOK.title, AUTHOR.id, AUTHOR.first_name, AUTHOR.last_name, AUTHOR.date_of_birth, AUTHOR.year_of_birth, AUTHOR.distinguished, BOOK.published_in})
    .from(BOOK)
    .join_left(AUTHOR)
    .on(BOOK.author_id == AUTHOR.id)
    .order_by(BOOK.id).asc()
    .fetch_all<book>(db);

  REQUIRE(books_result.is_ok());
  size_t index{0};
  for (const auto &b : *books_result) {
    REQUIRE(b->id == books.at(index)->id);
    REQUIRE(b->title == books.at(index)->title);
    REQUIRE(b->book_author->id == books.at(index)->book_author->id);
    REQUIRE(b->book_author->first_name == books.at(index)->book_author->first_name);
    REQUIRE(b->book_author->last_name == books.at(index)->book_author->last_name);
    REQUIRE(b->book_author->date_of_birth == books.at(index)->book_author->date_of_birth);
    REQUIRE(b->book_author->year_of_birth == books.at(index)->book_author->year_of_birth);
    REQUIRE(b->book_author->distinguished == books.at(index)->book_author->distinguished);
    REQUIRE(b->published_in == books.at(index)->published_in);
    ++index;
  }
}

TEST_CASE_METHOD(QueryFixture, "Test load entity with eager has many to many relation", "[query][has_many_to_many][eager]") {
  auto result = repo.attach<ingredient>("ingredients")
    .and_then( [this] { return repo.attach<recipe>("recipes"); } )
    .and_then([this] {return manager.create(db); });

  REQUIRE(result.is_ok());

  initialize_resolver(repo, db);

  REQUIRE(db.exists(RECIPE.table_name()));
  REQUIRE(db.exists(INGREDIENT.table_name()));
  REQUIRE(db.exists(RECIPE_INGREDIENT.table_name()));

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
    auto res = insert()
    .into(INGREDIENT, {INGREDIENT.id, INGREDIENT.name})
    .values(i)
    .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  std::vector<recipe> recipes{
        {8, "Apple Crumble"},
        {9, "Beans Chili"},
        {10, "Fruit Salad"}
  };

  for (const auto &r: recipes) {
    auto res = insert()
    .into(RECIPE, {RECIPE.id, RECIPE.name})
    .values(r)
    .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  std::vector<std::pair<int, int>> recipe_ingredients {
        { 8, 1 },
        { 8, 4 },
        { 8, 5 },
        { 9, 6 },
        { 9, 7 },
        { 10, 1 },
        { 10, 2 },
        { 10, 3 }
  };

  for (const auto & [recipe_id, ingredient_id]: recipe_ingredients) {
    auto res = insert()
    .into(RECIPE_INGREDIENT, {RECIPE_INGREDIENT.recipe_id, RECIPE_INGREDIENT.ingredient_id})
    .values({recipe_id, ingredient_id})
    .execute(db);
    REQUIRE(res.is_ok());
    REQUIRE(res->affected_rows == 1);
  }

  const auto r = RECIPE.as("r");
  const auto ri= RECIPE_INGREDIENT.as("ri");
  const auto i = INGREDIENT.as("i");

  auto recipes_result = select({r.id, r.name })
    .from(r)
    .order_by({r.id}).asc()
    .fetch_all<recipe>(db);
  REQUIRE(recipes_result.is_ok());

  for (const auto &reps : *recipes_result) {
    REQUIRE(!reps->ingredients.empty());
    for (const auto &ingr : reps->ingredients) {
      std::cout << ingr->name << " (" << reps->name << ")" << std::endl;
    }
  }

  auto ingredients_result = select({i.id, i.name, ri.recipe_id, r.name})
    .from(i)
    .join_left(ri).on(i.id == ri.ingredient_id)
    .join_left(r).on(ri.recipe_id == r.id)
    .order_by({i.id, r.id}).asc()
    .fetch_all<ingredient>(db);
  REQUIRE(ingredients_result.is_ok());

  for (const auto &ingr : *ingredients_result) {
    REQUIRE(!ingr->recipes.empty());
    for (const auto &recipe : ingr->recipes) {
      std::cout << recipe->name << " (" << ingr->name << ")" << std::endl;
    }
  }
}