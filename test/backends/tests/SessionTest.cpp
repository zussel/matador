#include "catch2/catch_test_macros.hpp"

#include "fixtures/SessionFixture.hpp"

#include "connection.hpp"

#include "matador/query/runtime/session.hpp"

#include "models/airplane.hpp"
#include "models/author.hpp"
#include "models/book.hpp"
#include "models/department.hpp"
#include "models/flight.hpp"
#include "models/recipe.hpp"

#include <iostream>

using namespace matador;
using namespace matador::query;
using namespace matador::test;

TEST_CASE_METHOD(SessionFixture, "Session insert test", "[session][insert]") {
  const auto result = schema.attach<airplane>("airplanes")
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);
  auto plane = ses.insert(make_object<airplane>(1, "Boeing", "A380"));
  REQUIRE(plane.is_ok());

  const auto res = ses.find<airplane>(1);
  REQUIRE(res.is_ok());
  const auto& read_airplane = *res;
  REQUIRE(read_airplane->id == (*plane)->id);
  REQUIRE(read_airplane->brand == (*plane)->brand);
  REQUIRE(read_airplane->model == (*plane)->model);
}

TEST_CASE_METHOD(SessionFixture, "Session update test", "[session][update]") {
  const auto res = schema.attach<airplane>("airplanes")
    .and_then([this] { return manager.create(db); } );
  REQUIRE(res.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);
  auto result = ses.insert(make_object<airplane>(1, "Boeing", "A380"));
  REQUIRE(result.is_ok());

  const auto plane = result.value();
  result = ses.find<airplane>(1);
  REQUIRE(result.is_ok());
  auto read_airplane = *result;
  REQUIRE(read_airplane->id == plane->id);
  REQUIRE(read_airplane->brand == plane->brand);
  REQUIRE(read_airplane->model == plane->model);

  read_airplane->brand = "Airbus";
  read_airplane->model = "A380";

  auto update_result = ses.update(read_airplane);
  REQUIRE(update_result.is_ok());

  result = ses.find<airplane>(1);
  REQUIRE(result.is_ok());
  read_airplane = *result;
  REQUIRE(read_airplane->id == plane->id);
  REQUIRE(read_airplane->brand == "Airbus");
  REQUIRE(read_airplane->model == "A380");
}

TEST_CASE_METHOD(SessionFixture, "Session delete test", "[session][delete]") {
  const auto res = schema.attach<airplane>("airplanes")
    .and_then([this] { return manager.create(db); } );
  REQUIRE(res.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);

  auto result = ses.insert(make_object<airplane>(1, "Boeing", "A380"));
  REQUIRE(result.is_ok());

  const auto plane = result.value();
  result = ses.find<airplane>(1);
  REQUIRE(result.is_ok());
  auto read_airplane = *result;

  auto update_result = ses.remove(read_airplane);
  REQUIRE(update_result.is_ok());

  result = ses.find<airplane>(1);
  REQUIRE(result.is_error());
  REQUIRE(result.err().ec() == query::error_code::FetchFailed);
}

TEST_CASE_METHOD(SessionFixture, "Session relation test", "[session][relation]") {
  const auto result = schema.attach<airplane>("airplanes")
    .and_then([this] { return schema.attach<flight>("flights"); } )
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);
  auto plane = ses.insert(make_object<airplane>(1, "Boeing", "A380"));
  REQUIRE(plane.is_ok());
  auto f = ses.insert(make_object<flight>(2, *plane, "sully"));
  REQUIRE(f.is_ok());

  const auto res = ses.find<flight>(2);
  REQUIRE(res.is_ok());
  const auto& rf = *res;
  REQUIRE(rf->id == (*f)->id);
  REQUIRE(rf->pilot_name == (*f)->pilot_name);
  REQUIRE(rf->plane);
  REQUIRE(rf->plane->id == (*plane)->id);
  REQUIRE(rf->plane->brand == (*plane)->brand);
  REQUIRE(rf->plane->model == (*plane)->model);
}

TEST_CASE_METHOD(SessionFixture, "Use session to find object with id", "[session][find]") {
  const auto result = schema.attach<airplane>("airplanes")
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);
  auto a380 = ses.insert(make_object<airplane>(1, "Boeing", "A380"));
  REQUIRE(a380.is_ok());

  auto find_result = ses.find<airplane>(2);
  REQUIRE(!find_result.is_ok());
  REQUIRE((find_result.err().ec() == query::error_code::FetchFailed));

  find_result = ses.find<airplane>(1);

  REQUIRE(find_result);
  auto read_a380 = find_result.value();
  REQUIRE((*a380)->id == read_a380->id);
}

TEST_CASE_METHOD(SessionFixture, "Use session to find all objects", "[session][find]") {
  const auto result = schema.attach<airplane>("airplanes")
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);

  std::vector planes {
    make_object<airplane>(1, "Airbus", "A380"),
    make_object<airplane>(2, "Boeing", "707"),
    make_object<airplane>(3, "Boeing", "747"),
  };

  for (auto &&plane: planes) {
    auto res = ses.insert(plane);
    REQUIRE(res.is_ok());
  }

  auto find_result = ses.find<airplane>();

  std::vector<std::tuple<unsigned long, std::string, std::string>> expected_result {
    {1, "Airbus", "A380"},
    {2, "Boeing", "707"},
    {3, "Boeing", "747"}
  };
  REQUIRE(find_result);
  auto all_planes = find_result.release();
  size_t index {0};
  for (const auto &i: all_planes) {
    REQUIRE(i->id == std::get<0>(expected_result[index]));
    REQUIRE(i->brand == std::get<1>(expected_result[index]));
    REQUIRE(i->model == std::get<2>(expected_result[index]));
    ++index;
  }
}

TEST_CASE_METHOD(SessionFixture, "Use session to find all objects with one-to-many lazy relation", "[session][find][one-to-many][eager]") {
  auto result = schema.attach<book>("books")
    .and_then( [this] { return schema.attach<author>("authors"); } )
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);
  std::vector authors {
    make_object<author>(1, "Michael", "Crichton", "23.10.1942", 1975, true),
    make_object<author>( 2, "Steven", "King", "21.9.1947", 1956, false)
  };

  for (const auto &a: authors) {
      auto res = ses.insert(a);
      REQUIRE(res.is_ok());
  }

  auto find_result = ses.find<author>();
  REQUIRE(find_result.is_ok());
  auto all_authors = find_result.release();
  std::vector<query::object_ptr<author>> author_repo;
  for (auto it = all_authors.begin(); it != all_authors.end(); ++it) {
    REQUIRE((*it)->books.empty());
    author_repo.emplace_back(*it);
  }
  REQUIRE(author_repo.size() == 2);

  std::vector books {
    make_object<book>(1, "Jurassic Park", author_repo[0], 1990),
    make_object<book>(2, "Timeline", author_repo[0], 1999),
    make_object<book>(3, "The Andromeda Strain", author_repo[0], 1969),
    make_object<book>(4, "Congo", author_repo[0], 1980),
    make_object<book>(5, "Prey", author_repo[0], 2002),
    make_object<book>(6, "Carrie", author_repo[1], 1974),
    make_object<book>(7, "The Shining", author_repo[1], 1977),
    make_object<book>(8, "It", author_repo[1], 1986),
    make_object<book>(9, "Misery", author_repo[1], 1987),
    make_object<book>(10, "The Dark Tower: The Gunslinger", author_repo[1], 1982)
  };

  for (auto &&b: books) {
    auto res = ses.insert(b);
    REQUIRE(res.is_ok());
  }

  find_result = ses.find<author>();
  REQUIRE(find_result);

  all_authors = find_result.release();
  for (auto it = all_authors.begin(); it != all_authors.end(); ++it) {
    REQUIRE((*it)->books.size() == 5);
  }
}

TEST_CASE_METHOD(SessionFixture, "Use session to find all objects with one-to-many eager relation", "[session][find][one-to-many][eager]") {
  auto result = schema.attach<employee>("employees")
    .and_then([this] { return schema.attach<department>("departments"); })
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);

  std::vector departments {
    make_object<department>(1, "Insurance"),
    make_object<department>(2, "Invoice")
  };

  for (auto &&a: departments) {
    auto res = ses.insert(a);
    REQUIRE(res.is_ok());
  }

  auto find_result = ses.find<department>();
  REQUIRE(find_result.is_ok());
  auto all_departments = find_result.release();
  std::vector<query::object_ptr<department>> departments_repo;
  for (auto it = all_departments.begin(); it != all_departments.end(); ++it) {
    REQUIRE((*it)->employees.empty());
    departments_repo.emplace_back(*it);
  }
  REQUIRE(departments_repo.size() == 2);

  std::vector employees {
    make_object<employee>(1, "George", "Orwell", departments_repo[0]),
    make_object<employee>(2, "Chris", "Tucker", departments_repo[0]),
    make_object<employee>(3, "Steven", "King", departments_repo[0]),
    make_object<employee>(4, "John", "Wayne", departments_repo[0]),
    make_object<employee>(5, "Clint", "Eastwood", departments_repo[0]),
    make_object<employee>(6, "Emma", "Thompson", departments_repo[1]),
    make_object<employee>(7, "Ed", "Wood", departments_repo[1]),
    make_object<employee>(8, "Steven", "Spielberg", departments_repo[1]),
    make_object<employee>(9, "Jane", "Fonda", departments_repo[1]),
    make_object<employee>(10, "Julia", "Roberts", departments_repo[1])
  };

  for (auto &&b: employees) {
    auto res = ses.insert(b);
    REQUIRE(res.is_ok());
  }

  find_result = ses.find<department>();
  REQUIRE(find_result);

  all_departments = find_result.release();
  for (auto it = all_departments.begin(); it != all_departments.end(); ++it) {
    REQUIRE((*it)->employees.size() == 5);
    // for (const auto& emp : it->employees) {
      // REQUIRE(emp->dep->id == it->id);
    // }
  }
}

TEST_CASE_METHOD(SessionFixture, "Use session to find all objects with many-to-many eager relation", "[session][find][many-to-many][eager]") {
  auto result = schema.attach<recipe>("recipes")
    .and_then( [this] { return schema.attach<ingredient>("ingredients"); } )
    .and_then([this] {
      return manager.create(db);
    } );

  session ses({bus, test::connection::dns, 4}, schema);

  std::vector ingredients {
    make_object<ingredient>(1, "Apple"),
    make_object<ingredient>(2, "Strawberry"),
    make_object<ingredient>(3, "Pineapple"),
    make_object<ingredient>(4, "Sugar"),
    make_object<ingredient>(5, "Flour"),
    make_object<ingredient>(6, "Butter"),
    make_object<ingredient>(7, "Beans")
  };

  for (auto &i: ingredients) {
    auto res = ses.insert(i);
    REQUIRE(res.is_ok());
  }

  std::vector recipes {
    make_object<recipe>(1, "Apple Pie", std::vector{ingredients[0], ingredients[3], ingredients[4]}),
    make_object<recipe>(2, "Strawberry Cake", std::vector{ingredients[5], ingredients[6]}),
    make_object<recipe>(3, "Pineapple Pie", std::vector{ingredients[0], ingredients[1], ingredients[2]})
  };

  for (auto &r: recipes) {
    auto res = ses.insert(r);
    REQUIRE(res.is_ok());
  }

  auto find_result = ses.find<ingredient>();
  REQUIRE(find_result.is_ok());
  auto all_ingredients = find_result.release();
  std::vector<query::object_ptr<ingredient>> ingredients_repo;
  std::vector<size_t> expected_ingredients_sizes {2, 1, 1, 1, 1, 1, 1};
  size_t index {0};
  for (auto it = all_ingredients.begin(); it != all_ingredients.end(); ++it) {
    REQUIRE(!(*it)->recipes.empty());
    REQUIRE((*it)->recipes.size() == expected_ingredients_sizes[index++]);
    ingredients_repo.emplace_back(*it);
  }
  REQUIRE(ingredients_repo.size() == ingredients.size());
}

TEST_CASE_METHOD(SessionFixture, "Use session to find all objects with many-to-many lazy relation", "[session][find][many-to-many][lazy]") {
  auto result = schema.attach<recipe>("recipes")
    .and_then( [this] { return schema.attach<ingredient>("ingredients"); } )
    .and_then([this] {
      return manager.create(db);
    } );

  session ses({bus, test::connection::dns, 4}, schema);

  std::vector ingredients {
    make_object<ingredient>(1, "Apple"),
    make_object<ingredient>(2, "Strawberry"),
    make_object<ingredient>(3, "Pineapple"),
    make_object<ingredient>(4, "Sugar"),
    make_object<ingredient>(5, "Flour"),
    make_object<ingredient>(6, "Butter"),
    make_object<ingredient>(7, "Beans")
  };

  for (auto &i: ingredients) {
    auto res = ses.insert(i);
    REQUIRE(res.is_ok());
  }

  std::vector recipes {
    make_object<recipe>(1, "Apple Pie", std::vector{ingredients[0], ingredients[3], ingredients[4]}),
    make_object<recipe>(2, "Strawberry Cake", std::vector{ingredients[5], ingredients[6]}),
    make_object<recipe>(3, "Pineapple Pie", std::vector{ingredients[0], ingredients[1], ingredients[2]})
  };

  for (auto &r: recipes) {
    auto res = ses.insert(r);
    REQUIRE(res.is_ok());
  }

  auto find_result = ses.find<recipe>();
  REQUIRE(find_result.is_ok());
  auto all_recipes = find_result.release();
  std::vector<query::object_ptr<recipe>> recipes_repo;
  std::vector<size_t> expected_recipe_sizes {3, 2, 3};
  size_t index {0};
  for (auto it = all_recipes.begin(); it != all_recipes.end(); ++it) {
    REQUIRE(!(*it)->ingredients.empty());
    REQUIRE((*it)->ingredients.size() == expected_recipe_sizes[index++]);
    recipes_repo.emplace_back(*it);
  }
  REQUIRE(recipes_repo.size() == recipes.size());

}