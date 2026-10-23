#include <catch2/catch_test_macros.hpp>

#include "matador/query/dialect.hpp"
#include "matador/query/internal/insert_query_builder.hpp"
#include "matador/query/internal/schema_utils.hpp"
#include "matador/query/schema.hpp"

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

std::string insert_sql(const execute_step &step) {
  return step.ctx().sql;
}

} // namespace

TEST_CASE("Insert query builder reports unknown root types", "[query][insert_builder]") {
  schema scm;
  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  insert_query_builder<department> builder(scm, contexts);

  const auto dep = make_object<department>(1U, "sales");
  const auto result = builder.build(dep);

  REQUIRE(!result.is_ok());
  REQUIRE(result.err().ec() == error_code::UnknownType);
}

TEST_CASE("Insert query builder reports null objects", "[query][insert_builder]") {
  schema scm;
  REQUIRE(scm.attach<department>("departments").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  insert_query_builder<department> builder(scm, contexts);

  const object_ptr<department> dep;
  const auto result = builder.build(dep);

  REQUIRE(!result.is_ok());
  REQUIRE(result.err().ec() == error_code::InvalidObject);
}

TEST_CASE("Insert query builder reports missing query contexts", "[query][insert_builder]") {
  schema scm;
  REQUIRE(scm.attach<department>("departments").is_ok());

  const std::unordered_map<std::type_index, query_contexts> contexts;
  insert_query_builder<department> builder(scm, contexts);

  const auto dep = make_object<department>(1U, "sales");
  const auto result = builder.build(dep);

  REQUIRE(!result.is_ok());
  REQUIRE(result.err().ec() == error_code::UnknownType);
}

TEST_CASE("Insert query builder creates a single insert step for a simple object", "[query][insert_builder]") {
  schema scm;
  REQUIRE(scm.attach<department>("departments").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  insert_query_builder<department> builder(scm, contexts);

  const auto dep = make_object<department>(1U, "sales");
  const auto result = builder.build(dep);

  REQUIRE(result.is_ok());
  REQUIRE(result->size() == 1);

  const auto sql = insert_sql(*result->front());
  INFO(sql);
  REQUIRE(sql.find("INSERT INTO \"departments\"") != std::string::npos);
  REQUIRE(sql.find("\"id\"") != std::string::npos);
  REQUIRE(sql.find("\"name\"") != std::string::npos);
}

TEST_CASE("Insert query builder inserts belongs_to dependencies before the root object", "[query][insert_builder]") {
  schema scm;
  REQUIRE(scm.attach<airplane>("airplanes").is_ok());
  REQUIRE(scm.attach<flight>("flights").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  insert_query_builder<flight> builder(scm, contexts);

  const auto plane = make_object<airplane>(7U, "Airbus", "A320");
  const auto flt = make_object<flight>(42U, plane, "Maverick");

  const auto result = builder.build(flt);

  REQUIRE(result.is_ok());
  REQUIRE(result->size() == 2);

  const auto first_sql = insert_sql(*(*result)[0]);
  const auto second_sql = insert_sql(*(*result)[1]);

  INFO(first_sql);
  INFO(second_sql);

  REQUIRE(first_sql.find("INSERT INTO \"airplanes\"") != std::string::npos);
  REQUIRE(second_sql.find("INSERT INTO \"flights\"") != std::string::npos);
}

TEST_CASE("Insert query builder inserts has_many children after parent and links the join column", "[query][insert_builder]") {
  schema scm;
  REQUIRE(scm.attach<department>("departments").is_ok());
  REQUIRE(scm.attach<employee>("employees").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  insert_query_builder<department> builder(scm, contexts);

  const auto dep = make_object<department>(1U, "engineering");
  const auto alice = make_object<employee>(2U, "Alice", "Smith", object_ptr<department>{});
  const auto bob = make_object<employee>(3U, "Bob", "Miller", object_ptr<department>{});

  dep->employees.push_back(alice);
  dep->employees.push_back(bob);

  const auto result = builder.build(dep);

  REQUIRE(result.is_ok());
  REQUIRE(result->size() == 3);

  const auto parent_sql = insert_sql(*(*result)[0]);
  const auto first_child_sql = insert_sql(*(*result)[1]);
  const auto second_child_sql = insert_sql(*(*result)[2]);

  INFO(parent_sql);
  INFO(first_child_sql);
  INFO(second_child_sql);

  REQUIRE(parent_sql.find("INSERT INTO \"departments\"") != std::string::npos);
  REQUIRE(first_child_sql.find("INSERT INTO \"employees\"") != std::string::npos);
  REQUIRE(second_child_sql.find("INSERT INTO \"employees\"") != std::string::npos);

  REQUIRE(alice->dep == dep);
  REQUIRE(bob->dep == dep);
}

TEST_CASE("Insert query builder inserts many-to-many foreign objects before relation rows", "[query][insert_builder]") {
  schema scm;
  REQUIRE(scm.attach<ingredient>("ingredients").is_ok());
  REQUIRE(scm.attach<recipe>("recipes").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  insert_query_builder<ingredient> builder(scm, contexts);

  const auto flour = make_object<ingredient>(1U, "flour");
  const auto pancakes = make_object<recipe>(2U, "pancakes");
  const auto bread = make_object<recipe>(3U, "bread");

  flour->recipes.push_back(pancakes);
  flour->recipes.push_back(bread);

  const auto result = builder.build(flour);

  REQUIRE(result.is_ok());
  REQUIRE(result->size() == 5);

  const auto root_sql = insert_sql(*(*result)[0]);
  const auto first_recipe_sql = insert_sql(*(*result)[1]);
  const auto second_recipe_sql = insert_sql(*(*result)[2]);
  const auto first_relation_sql = (*result)[3]->ctx().sql;
  const auto second_relation_sql = (*result)[4]->ctx().sql;

  INFO(root_sql);
  INFO(first_recipe_sql);
  INFO(second_recipe_sql);
  INFO(first_relation_sql);
  INFO(second_relation_sql);

  REQUIRE(root_sql.find("INSERT INTO \"ingredients\"") != std::string::npos);
  REQUIRE(first_recipe_sql.find("INSERT INTO \"recipes\"") != std::string::npos);
  REQUIRE(second_recipe_sql.find("INSERT INTO \"recipes\"") != std::string::npos);
  REQUIRE(first_relation_sql.find("INSERT INTO \"recipe_ingredients\"") != std::string::npos);
  REQUIRE(second_relation_sql.find("INSERT INTO \"recipe_ingredients\"") != std::string::npos);
}

TEST_CASE("Insert query builder can be reused without leaking visited relation state", "[query][insert_builder]") {
  schema scm;
  REQUIRE(scm.attach<ingredient>("ingredients").is_ok());
  REQUIRE(scm.attach<recipe>("recipes").is_ok());

  const auto dialect = dialect_builder::make_default();
  const auto contexts = make_contexts(scm, dialect);

  insert_query_builder<ingredient> builder(scm, contexts);

  const auto flour = make_object<ingredient>(1U, "flour");
  flour->recipes.push_back(make_object<recipe>(2U, "pancakes"));

  const auto sugar = make_object<ingredient>(3U, "sugar");
  sugar->recipes.push_back(make_object<recipe>(4U, "cake"));

  const auto first = builder.build(flour);
  const auto second = builder.build(sugar);

  REQUIRE(first.is_ok());
  REQUIRE(second.is_ok());

  REQUIRE(first->size() == 3);
  REQUIRE(second->size() == 3);

  REQUIRE((*first)[2]->ctx().sql.find("INSERT INTO \"recipe_ingredients\"") != std::string::npos);
  REQUIRE((*second)[2]->ctx().sql.find("INSERT INTO \"recipe_ingredients\"") != std::string::npos);
}