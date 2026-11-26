#include "catch2/catch_test_macros.hpp"

#include "fixtures/SessionFixture.hpp"

#include "connection.hpp"

#include "matador/query/runtime/session.hpp"

#include "models/recipe.hpp"
#include "models/model_metas.hpp"

using namespace matador;
using namespace matador::test;
using namespace matador::query::meta;

TEST_CASE_METHOD(SessionFixture, "Test insert object with has many to many relation", "[session][insert][has_many_to_many]") {
  auto result = schema.attach<recipe>("recipes")
    .and_then( [this] { return schema.attach<ingredient>("ingredients"); } )
    .and_then([this] { return manager.create(db); } );

  query::session ses({bus, connection::dns, 4}, schema);

  std::vector ingredients {
    query::make_object<ingredient>(1, "Apple"),
    query::make_object<ingredient>(2, "Strawberry"),
    query::make_object<ingredient>(3, "Pineapple"),
    query::make_object<ingredient>(4, "Sugar"),
    query::make_object<ingredient>(5, "Flour"),
    query::make_object<ingredient>(6, "Butter"),
    query::make_object<ingredient>(7, "Beans")
  };

  std::vector recipes {
    query::make_object<recipe>(1, "Apple Pie", std::vector{ingredients[0], ingredients[3], ingredients[4]}),
    query::make_object<recipe>(2, "Strawberry Cake", std::vector{ingredients[5], ingredients[6]}),
    query::make_object<recipe>(3, "Pineapple Pie", std::vector{ingredients[0], ingredients[1], ingredients[2]})
  };

  for (auto &r: recipes) {
    REQUIRE(r.is_transient());
    auto res = ses.insert(r);
    REQUIRE(res.is_ok());
    REQUIRE(res->is_persistent());
  }

  auto recipe_result = ses.find<recipe>(1);
  REQUIRE(recipe_result.is_ok());
  REQUIRE(recipe_result->is_persistent());
  REQUIRE(recipe_result.value()->ingredients.size() == 3);
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with has many to many relation identity", "[session][insert][identity][has_many_to_many]") {
  auto result = schema.attach<recipe_identity>("recipes")
    .and_then( [this] { return schema.attach<ingredient_identity>("ingredients"); } )
    .and_then([this] { return manager.create(db); } );

  query::session ses({bus, connection::dns, 4}, schema);

  std::vector ingredients {
    query::make_object<ingredient_identity>("Apple"),
    query::make_object<ingredient_identity>("Strawberry"),
    query::make_object<ingredient_identity>("Pineapple"),
    query::make_object<ingredient_identity>("Sugar"),
    query::make_object<ingredient_identity>("Flour"),
    query::make_object<ingredient_identity>("Butter"),
    query::make_object<ingredient_identity>("Beans")
  };

  std::vector recipes {
    query::make_object<recipe_identity>("Apple Pie", std::vector{ingredients[0], ingredients[3], ingredients[4]}),
    query::make_object<recipe_identity>("Strawberry Cake", std::vector{ingredients[5], ingredients[6]}),
    query::make_object<recipe_identity>("Pineapple Pie", std::vector{ingredients[0], ingredients[1], ingredients[2]})
  };

  for (auto &r: recipes) {
    REQUIRE(r.is_transient());
    auto res = ses.insert(r);
    REQUIRE(res.is_ok());
    REQUIRE(res->is_persistent());
  }

  auto recipe_result = ses.find<recipe_identity>(1);
  REQUIRE(recipe_result.is_ok());
  REQUIRE(recipe_result->is_persistent());
  REQUIRE(recipe_result.value()->ingredients.size() == 3);
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with has many to many relation table sequence", "[session][insert][table_sequence][has_many_to_many]") {
  auto result = schema.attach<recipe_table>("recipes")
    .and_then( [this] { return schema.attach<ingredient_table>("ingredients"); } )
    .and_then([this] { return manager.create(db); } );

  query::session ses({bus, connection::dns, 4}, schema);

  std::vector ingredients {
    query::make_object<ingredient_table>("Apple"),
    query::make_object<ingredient_table>("Strawberry"),
    query::make_object<ingredient_table>("Pineapple"),
    query::make_object<ingredient_table>("Sugar"),
    query::make_object<ingredient_table>("Flour"),
    query::make_object<ingredient_table>("Butter"),
    query::make_object<ingredient_table>("Beans")
  };

  std::vector recipes {
    query::make_object<recipe_table>("Apple Pie", std::vector{ingredients[0], ingredients[3], ingredients[4]}),
    query::make_object<recipe_table>("Strawberry Cake", std::vector{ingredients[5], ingredients[6]}),
    query::make_object<recipe_table>("Pineapple Pie", std::vector{ingredients[0], ingredients[1], ingredients[2]})
  };

  for (auto &r: recipes) {
    REQUIRE(r.is_transient());
    auto res = ses.insert(r);
    REQUIRE(res.is_ok());
    REQUIRE(res->is_persistent());
  }

  auto recipe_result = ses.find<recipe_table>(1);
  REQUIRE(recipe_result.is_ok());
  REQUIRE(recipe_result->is_persistent());
  REQUIRE(recipe_result.value()->ingredients.size() == 3);
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with has many to many relation sequence", "[session][insert][sequence][has_many_to_many]") {
  auto result = schema.attach<recipe_sequence>("recipes")
    .and_then( [this] { return schema.attach<ingredient_sequence>("ingredients"); } )
    .and_then([this] { return manager.create(db); } );

  query::session ses({bus, connection::dns, 4}, schema);

  std::vector ingredients {
    query::make_object<ingredient_sequence>("Apple"),
    query::make_object<ingredient_sequence>("Strawberry"),
    query::make_object<ingredient_sequence>("Pineapple"),
    query::make_object<ingredient_sequence>("Sugar"),
    query::make_object<ingredient_sequence>("Flour"),
    query::make_object<ingredient_sequence>("Butter"),
    query::make_object<ingredient_sequence>("Beans")
  };

  std::vector recipes {
    query::make_object<recipe_sequence>("Apple Pie", std::vector{ingredients[0], ingredients[3], ingredients[4]}),
    query::make_object<recipe_sequence>("Strawberry Cake", std::vector{ingredients[5], ingredients[6]}),
    query::make_object<recipe_sequence>("Pineapple Pie", std::vector{ingredients[0], ingredients[1], ingredients[2]})
  };

  for (auto &r: recipes) {
    REQUIRE(r.is_transient());
    auto res = ses.insert(r);
    REQUIRE(res.is_ok());
    REQUIRE(res->is_persistent());
  }

  auto recipe_result = ses.find<recipe_sequence>(1);
  REQUIRE(recipe_result.is_ok());
  REQUIRE(recipe_result->is_persistent());
  REQUIRE(recipe_result.value()->ingredients.size() == 3);

  const auto ing_result = ses.find<ingredient_sequence>(ingredients[0]->id);
  REQUIRE(ing_result.is_ok());
  REQUIRE(ing_result.value()->recipes.size() == 2);
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with has many to many relation sequence reverse", "[session][insert][sequence][has_many_to_many][reverse]") {
  auto result = schema.attach<recipe_sequence>("recipes")
    .and_then( [this] { return schema.attach<ingredient_sequence>("ingredients"); } )
    .and_then([this] { return manager.create(db); } );

  query::session ses({bus, connection::dns, 4}, schema);

  std::vector recipes {
    query::make_object<recipe_sequence>("Apple Pie"),
    query::make_object<recipe_sequence>("Strawberry Cake"),
    query::make_object<recipe_sequence>("Pineapple Pie")
  };

  std::vector ingredients {
    query::make_object<ingredient_sequence>("Apple", std::vector{recipes[0], recipes[2]}),
    query::make_object<ingredient_sequence>("Strawberry", std::vector{recipes[2]}),
    query::make_object<ingredient_sequence>("Pineapple", std::vector{recipes[2]}),
    query::make_object<ingredient_sequence>("Sugar", std::vector{recipes[0]}),
    query::make_object<ingredient_sequence>("Flour", std::vector{recipes[0]}),
    query::make_object<ingredient_sequence>("Butter", std::vector{recipes[1]}),
    query::make_object<ingredient_sequence>("Beans", std::vector{recipes[1]})
  };

  for (auto &i: ingredients) {
    REQUIRE(i.is_transient());
    auto res = ses.insert(i);
    REQUIRE(res.is_ok());
    REQUIRE(res->is_persistent());
  }

  auto recipe_result = ses.find<ingredient_sequence>(1);
  REQUIRE(recipe_result.is_ok());
  REQUIRE(recipe_result->is_persistent());
  REQUIRE(recipe_result.value()->recipes.size() == 2);

  const auto ing_result = ses.find<recipe_sequence>(recipes[0]->id);
  REQUIRE(ing_result.is_ok());
  REQUIRE(ing_result.value()->ingredients.size() == 3);
}
