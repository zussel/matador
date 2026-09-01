#include <catch2/catch_test_macros.hpp>

#include "matador/query/schema.hpp"

#include "models/department.hpp"
#include "models/recipe.hpp"
#include "models/person.hpp"

#include <iostream>

using namespace matador;

namespace {
struct person {
  virtual ~person() = default;

  template<typename Operator>
  static void process(Operator &/*op*/) {}
};

struct student final : person {};

struct teacher final : person {};
}

struct names {
  unsigned int id{};
  std::vector<std::string> names_list;

  template<typename Operator>
  void process(Operator &op) {
    query::field::primary_key(op, "id", id);
    query::field::has_many(op, "name_list", names_list, "names_id", query::fetch_type::Eager);
  }
};

TEST_CASE("Test empty prototype tree", "[schema_node][empty]") {
  const query::schema repo;

  REQUIRE(repo.empty());
}

TEST_CASE("Test add type to prototype tree", "[schema_node][add]") {
  query::schema repo;

  REQUIRE(repo.empty());

  auto res = repo.attach<person>("person");
  REQUIRE(res.is_ok());
  res = repo.attach<student, person>("student");
  REQUIRE(res.is_ok());
  res = repo.attach<teacher, person>("teacher");
  REQUIRE(res.is_ok());

  REQUIRE(!repo.empty());
  REQUIRE(repo.size() == 3);
}

TEST_CASE("Test next and previous of schema node", "[schema_node][next][previous]") {
  query::schema repo;

  REQUIRE(repo.empty());

  auto res = repo.attach<person>("person");
  REQUIRE(res.is_ok());

  REQUIRE(repo.size() == 1);

  auto it = repo.begin();
  REQUIRE(it->name() == "person");
  REQUIRE((--it)->name() == "person");
  REQUIRE(++it == repo.end());
}

TEST_CASE("Test automatic creating of a relation table with values", "[schema][relation_table][values]") {
  query::schema repo;

  REQUIRE(repo.empty());

  auto res = repo.attach<names>("names");
  REQUIRE(res.is_ok());
}

TEST_CASE("Test one to many", "[relation][one-to-many]") {
  query::schema repo;

  REQUIRE(repo.empty());

  auto res = repo.attach<test::department>("departments")
    .and_then([&repo] { return repo.attach<test::employee>("employees"); });
  REQUIRE(res.is_ok());
  REQUIRE(repo.size() == 2);
  REQUIRE(repo.contains("departments"));
  REQUIRE(repo.contains("employees"));

  auto result = repo.basic_info("departments");
  REQUIRE(!result->get().endpoints_empty());
  REQUIRE(result->get().endpoints_size() == 1);
  // std::cout << *result->get().table();
  result = repo.basic_info("employees");
  REQUIRE(!result->get().endpoints_empty());
  REQUIRE(result->get().endpoints_size() == 1);
  // std::cout << *result->get().object();
}

TEST_CASE("Test one to many reverse", "[relation][one-to-many][reverse]") {
  query::schema repo;

  REQUIRE(repo.empty());

  auto res = repo.attach<test::employee>("employees")
    .and_then([&repo] { return repo.attach<test::department>("departments"); });
  REQUIRE(res.is_ok());
  REQUIRE(repo.size() == 2);
  REQUIRE(repo.contains("departments"));
  REQUIRE(repo.contains("employees"));

  auto result = repo.basic_info("departments");
  REQUIRE(result.is_ok());
  REQUIRE(!result->get().endpoints_empty());
  REQUIRE(result->get().endpoints_size() == 1);
  // std::cout << *result->get().object();
  result = repo.basic_info("employees");
  REQUIRE(!result->get().endpoints_empty());
  REQUIRE(result->get().endpoints_size() == 1);
  // std::cout << *result->get().object();
}

TEST_CASE("Test many to many relation", "[relation][many-to-many]") {
  query::schema repo;

  REQUIRE(repo.empty());

  auto res = repo.attach<test::recipe>("recipes")
      .and_then([&repo] { return repo.attach<test::ingredient>("ingredients"); });
  REQUIRE(res);

  REQUIRE(repo.size() == 3);
  REQUIRE(repo.contains("ingredients"));
  REQUIRE(repo.contains("recipes"));
  REQUIRE(repo.contains("recipe_ingredients"));

  // std::cout << *repo.basic_info("ingredients")->get().object();
  // std::cout << *repo.basic_info("recipes")->get().object();
  // std::cout << *repo.basic_info("recipe_ingredients")->get().object();


  auto result = repo.basic_info("ingredients");
  REQUIRE(!result->get().endpoints_empty());
  REQUIRE(result->get().endpoints_size() == 1);
  // std::cout << *result->get().object();
  result = repo.basic_info("recipes");
  REQUIRE(!result->get().endpoints_empty());
  REQUIRE(result->get().endpoints_size() == 1);
  // std::cout << *result->get().object();
  result = repo.basic_info("recipe_ingredients");
  REQUIRE(!result->get().endpoints_empty());
  REQUIRE(result->get().endpoints_size() == 2);
  // std::cout << *result->get().object();
}

template<typename Type>
class test_observer : public matador::query::observer<Type> {
public:
  void on_attach(const query::schema_node& /*node*/, const Type& /*prototype*/) const override {}
  void on_detach(const query::schema_node& /*node*/, const Type& /*prototype*/) const override {}
  void on_insert(const Type& /*obj*/) override {}
  void on_update(const Type& /*obj*/) override {}
  void on_delete(const Type& /*obj*/) override {}
};

TEST_CASE("Test repository observer", "[repository][observer]") {
  query::schema repo;

  REQUIRE(repo.empty());

  const auto result = repo.attach<test::person>("person", test_observer<test::person>());
  REQUIRE(result.is_ok());
}