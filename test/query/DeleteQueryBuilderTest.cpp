#include <catch2/catch_test_macros.hpp>

#include "matador/query/schema/schema.hpp"
#include "matador/query/dialect/dialect.hpp"
#include "matador/query/internal/delete_query_builder.hpp"
#include "matador/query/internal/schema_utils.hpp"

#include "models/department.hpp"
#include "models/flight.hpp"
#include "models/recipe.hpp"

#include <string>
#include <typeindex>
#include <unordered_map>

using namespace matador::query;
using namespace matador::test;

namespace matador::query {
class dialect_builder {
public:
  static dialect make_default() {
    return {};
  }
};
}

namespace {

std::unordered_map<std::type_index, query_contexts> make_contexts(const schema &scm, const dialect &d) {
  std::unordered_map<std::type_index, query_contexts> contexts;

  for (const auto &node : scm) {
    contexts.emplace(node.type_index(), to_query_contexts(node, d));
  }

  return contexts;
}

std::string delete_sql(const execute_step &step) {
  return step.ctx().sql;
}

} // namespace

TEST_CASE("Delete query builder reports unknown root types", "[query][delete_builder]") {
  schema scm;
  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  delete_query_builder<department> builder(scm, contexts);

  const auto dep = make_object<department>(1U, "sales");
  const auto result = builder.build(dep);

  REQUIRE(!result.is_ok());
  REQUIRE(result.err().ec() == error_code::UnknownType);
}

TEST_CASE("Delete query builder reports null objects", "[query][delete_builder]") {
  schema scm;
  REQUIRE(scm.attach<department>("departments").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  delete_query_builder<department> builder(scm, contexts);

  const object_ptr<department> dep;
  const auto result = builder.build(dep);

  REQUIRE(!result.is_ok());
  REQUIRE(result.err().ec() == error_code::InvalidObject);
}

TEST_CASE("Delete query builder reports missing query contexts", "[query][delete_builder]") {
  schema scm;
  REQUIRE(scm.attach<department>("departments").is_ok());

  const std::unordered_map<std::type_index, query_contexts> contexts;
  delete_query_builder<department> builder(scm, contexts);

  const auto dep = make_object<department>(1U, "sales");
  const auto result = builder.build(dep);

  REQUIRE(!result.is_ok());
  REQUIRE(result.err().ec() == error_code::UnknownType);
}

TEST_CASE("Delete query builder creates a single delete step for a simple object", "[query][delete_builder]") {
  schema scm;
  REQUIRE(scm.attach<department>("departments").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  delete_query_builder<department> builder(scm, contexts);

  const auto dep = make_object<department>(1U, "sales");
  const auto result = builder.build(dep);

  REQUIRE(result.is_ok());
  REQUIRE(result->size() == 1);

  const auto sql = delete_sql(*result->front());

  INFO(sql);
  REQUIRE(sql.find("DELETE FROM \"departments\"") != std::string::npos);
  REQUIRE(sql.find("WHERE \"departments\".\"id\" = ?") != std::string::npos);
}

TEST_CASE("Delete query builder deletes belongs_to cascades before the root object", "[query][delete_builder]") {
  schema scm;
  REQUIRE(scm.attach<airplane>("airplanes").is_ok());
  REQUIRE(scm.attach<flight>("flights").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  delete_query_builder<flight> builder(scm, contexts);

  const auto plane = make_object<airplane>(7U, "Airbus", "A320");
  const auto flt = make_object<flight>(42U, plane, "Maverick");

  const auto result = builder.build(flt);

  REQUIRE(result.is_ok());
  REQUIRE(result->size() == 2);

  const auto first_sql = delete_sql(*(*result)[0]);
  const auto second_sql = delete_sql(*(*result)[1]);

  INFO(first_sql);
  INFO(second_sql);

  REQUIRE(first_sql.find("DELETE FROM \"airplanes\"") != std::string::npos);
  REQUIRE(second_sql.find("DELETE FROM \"flights\"") != std::string::npos);
}

TEST_CASE("Delete query builder deletes has_many children before the parent object", "[query][delete_builder]") {
  schema scm;
  REQUIRE(scm.attach<department>("departments").is_ok());
  REQUIRE(scm.attach<employee>("employees").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  delete_query_builder<department> builder(scm, contexts);

  const auto dep = make_object<department>(1U, "engineering");
  dep->employees.push_back(make_object<employee>(2U, "Alice", "Smith", dep));
  dep->employees.push_back(make_object<employee>(3U, "Bob", "Miller", dep));

  const auto result = builder.build(dep);

  REQUIRE(result.is_ok());
  REQUIRE(result->size() == 3);

  const auto first_child_sql = delete_sql(*(*result)[0]);
  const auto second_child_sql = delete_sql(*(*result)[1]);
  const auto parent_sql = delete_sql(*(*result)[2]);

  INFO(first_child_sql);
  INFO(second_child_sql);
  INFO(parent_sql);

  REQUIRE(first_child_sql.find("DELETE FROM \"employees\"") != std::string::npos);
  REQUIRE(second_child_sql.find("DELETE FROM \"employees\"") != std::string::npos);
  REQUIRE(parent_sql.find("DELETE FROM \"departments\"") != std::string::npos);
}

TEST_CASE("Delete query builder skips duplicate objects during cascading deletes", "[query][delete_builder]") {
  schema scm;
  REQUIRE(scm.attach<department>("departments").is_ok());
  REQUIRE(scm.attach<employee>("employees").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  delete_query_builder<department> builder(scm, contexts);

  const auto dep = make_object<department>(1U, "engineering");
  const auto alice = make_object<employee>(2U, "Alice", "Smith", dep);

  dep->employees.push_back(alice);
  dep->employees.push_back(alice);

  const auto result = builder.build(dep);

  REQUIRE(result.is_ok());
  REQUIRE(result->size() == 2);

  const auto child_sql = delete_sql(*(*result)[0]);
  const auto parent_sql = delete_sql(*(*result)[1]);

  INFO(child_sql);
  INFO(parent_sql);

  REQUIRE(child_sql.find("DELETE FROM \"employees\"") != std::string::npos);
  REQUIRE(parent_sql.find("DELETE FROM \"departments\"") != std::string::npos);
}

TEST_CASE("Delete query builder cascades many-to-many persistent objects before root", "[query][delete_builder]") {
  schema scm;
  REQUIRE(scm.attach<ingredient>("ingredients").is_ok());
  REQUIRE(scm.attach<recipe>("recipes").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  delete_query_builder<ingredient> builder(scm, contexts);

  const auto flour = make_object<ingredient>(1U, "flour");
  const auto pancakes = make_object<recipe>(2U, "pancakes");
  const auto bread = make_object<recipe>(3U, "bread");

  pancakes.change_state(object_state::Persistent);
  bread.change_state(object_state::Persistent);

  flour->recipes.push_back(pancakes);
  flour->recipes.push_back(bread);

  const auto result = builder.build(flour);

  REQUIRE(result.is_ok());
  REQUIRE(result->size() == 5);

  const auto first_recipe_sql = delete_sql(*(*result)[1]);
  const auto second_recipe_sql = delete_sql(*(*result)[3]);
  const auto root_sql = delete_sql(*(*result)[4]);

  INFO(first_recipe_sql);
  INFO(second_recipe_sql);
  INFO(root_sql);

  REQUIRE(first_recipe_sql.find("DELETE FROM \"recipes\"") != std::string::npos);
  REQUIRE(second_recipe_sql.find("DELETE FROM \"recipes\"") != std::string::npos);
  REQUIRE(root_sql.find("DELETE FROM \"ingredients\"") != std::string::npos);
}

TEST_CASE("Delete query builder ignores transient many-to-many objects", "[query][delete_builder]") {
  schema scm;
  REQUIRE(scm.attach<ingredient>("ingredients").is_ok());
  REQUIRE(scm.attach<recipe>("recipes").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  delete_query_builder<ingredient> builder(scm, contexts);

  const auto flour = make_object<ingredient>(1U, "flour");
  flour->recipes.push_back(make_object<recipe>(2U, "pancakes"));
  flour->recipes.push_back(make_object<recipe>(3U, "bread"));

  const auto result = builder.build(flour);

  REQUIRE(result.is_ok());
  REQUIRE(result->size() == 1);

  const auto sql = delete_sql(*result->front());

  INFO(sql);
  REQUIRE(sql.find("DELETE FROM \"ingredients\"") != std::string::npos);
}

TEST_CASE("Test delete builder has many to many", "[query][delete_query_builder][many_to_many]") {
  schema scm;
  const auto result = scm.attach<recipe>("recipes")
    .and_then( [&scm] { return scm.attach<ingredient>("ingredients"); } );
  REQUIRE(result.is_ok());

  std::vector ingredients {
    make_object<ingredient>(1, "Apple"),
    make_object<ingredient>(2, "Strawberry"),
    make_object<ingredient>(3, "Pineapple"),
    make_object<ingredient>(4, "Sugar"),
    make_object<ingredient>(5, "Flour"),
    make_object<ingredient>(6, "Butter"),
    make_object<ingredient>(7, "Beans")
  };

  for (auto &i : ingredients) {
    i.change_state(object_state::Persistent);
  }

  std::vector recipes {
    make_object<recipe>(1, "Apple Pie", std::vector{ingredients[0], ingredients[3], ingredients[4]}),
    make_object<recipe>(2, "Strawberry Cake", std::vector{ingredients[5], ingredients[6]}),
    make_object<recipe>(3, "Pineapple Pie", std::vector{ingredients[0], ingredients[1], ingredients[2]})
  };

  for (auto &r : recipes) {
    r.change_state(object_state::Persistent);
  }

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);
  delete_query_builder<recipe> dqb(scm, contexts);

  auto build_result = dqb.build(recipes[0]);
  REQUIRE(build_result.is_ok());

  const auto& stmts = *build_result;
  REQUIRE_FALSE(stmts.empty());
  for (const auto &stmt : stmts) {
    std::cout << stmt->ctx().sql << std::endl;
  }
  REQUIRE(stmts.size() == 7);
}