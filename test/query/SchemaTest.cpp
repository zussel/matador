#include <catch2/catch_test_macros.hpp>

#include "matador/query/schema/schema.hpp"

#include "models/department.hpp"
#include "models/person.hpp"
#include "models/recipe.hpp"

using namespace matador;

namespace {
struct person {
  virtual ~person() = default;

  template<typename Operator>
  static void process(Operator &/*op*/) {}
};

struct student final : person {};

struct teacher final : person {};

  struct names {
  unsigned int id{};
  std::vector<std::string> names_list;

  template<typename Operator>
  void process(Operator &op) {
    query::access::primary_key(op, "id", id);
    query::access::has_many(op, "name_list", names_list, "names_id", query::fetch_type::Eager);
  }
};
}

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
  result = repo.basic_info("employees");
  REQUIRE(!result->get().endpoints_empty());
  REQUIRE(result->get().endpoints_size() == 1);
}

static void require_endpoint(const query::basic_table_info& info, const std::type_index& ti, query::relation_type type, const std::string& field_name) {
  const auto eit = info.find_relation_endpoint(ti);
  REQUIRE(eit != info.endpoints_end());
  const auto& ep = *eit->second;
  REQUIRE(eit->first == ti);
  REQUIRE(ep.type() == type);
  REQUIRE(ep.field_name() == field_name);
}

static void require_schema(query::schema& repo) {
  REQUIRE(repo.size() == 3);
  REQUIRE(repo.contains("ingredients"));
  REQUIRE(repo.contains("recipes"));
  REQUIRE(repo.contains("recipe_ingredients"));

  auto result = repo.basic_info("ingredients");
  REQUIRE(result);
  const auto& recipe_info = result->get();
  REQUIRE(!recipe_info.endpoints_empty());
  REQUIRE(recipe_info.endpoints_size() == 1);
  require_endpoint(recipe_info, typeid(test::recipe), query::relation_type::HasMany, "recipe_ingredients");

  result = repo.basic_info("recipes");
  REQUIRE(result);
  const auto& ingredients_info = result->get();
  REQUIRE(!ingredients_info.endpoints_empty());
  REQUIRE(ingredients_info.endpoints_size() == 1);
  require_endpoint(ingredients_info, typeid(test::ingredient), query::relation_type::HasMany, "recipe_ingredients");

  result = repo.basic_info("recipe_ingredients");
  REQUIRE(result);
  const auto& recipe_ingredients_info = result->get();
  REQUIRE(!recipe_ingredients_info.endpoints_empty());
  REQUIRE(recipe_ingredients_info.endpoints_size() == 2);
  require_endpoint(recipe_ingredients_info, typeid(test::recipe), query::relation_type::BelongsTo, "recipe_id");
  require_endpoint(recipe_ingredients_info, typeid(test::ingredient), query::relation_type::BelongsTo, "ingredient_id");
}

TEST_CASE("Test many to many relation", "[relation][many-to-many]") {
  query::schema repo;

  REQUIRE(repo.empty());

  const std::type_index ti = typeid(query::many_to_many_relation<test::ingredient, test::recipe>);
  REQUIRE(repo.attach<test::ingredient>("ingredients").is_ok());
  REQUIRE(repo.attach<test::recipe>("recipes").is_ok());

  require_schema(repo);

  repo.clear();

  REQUIRE(repo.empty());

  REQUIRE(repo.attach<test::recipe>("recipes").is_ok());
  REQUIRE(repo.attach<test::ingredient>("ingredients").is_ok());

  require_schema(repo);
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

namespace {
template <typename Type>
class test_observer final : public query::observer<Type> {
public:
  void on_attach(const query::schema_node & /*node*/, const Type & /*prototype*/) const override {}
  void on_detach(const query::schema_node & /*node*/, const Type & /*prototype*/) const override {}
  void on_insert(const Type & /*obj*/) override {}
  void on_update(const Type & /*obj*/) override {}
  void on_delete(const Type & /*obj*/) override {}
};
} // namespace

TEST_CASE("Test repository observer", "[repository][observer]") {
  query::schema repo;

  REQUIRE(repo.empty());

  const auto result = repo.attach<test::person>("person", test_observer<test::person>());
  REQUIRE(result.is_ok());
}