#include <catch2/catch_test_macros.hpp>

#include "matador/query/collection.hpp"
#include "matador/query/error_code.hpp"
#include "matador/query/object_ptr.hpp"
#include "matador/query/schema.hpp"
#include "matador/query/relation_endpoint.hpp"
#include "matador/query/access.hpp"

#include <string>
#include <typeindex>

namespace {

using matador::query::collection;
using matador::query::object_ptr;
using matador::query::schema;
using matador::query::relation_type;

template<typename Type>
auto require_info(schema &repo) {
  auto result = repo.basic_info(std::type_index(typeid(Type)));
  REQUIRE(result.is_ok());
  return result;
}

auto require_info(schema &repo, const std::string &name) {
  auto result = repo.basic_info(name);
  REQUIRE(result.is_ok());
  return result;
}

template<typename InfoResult>
void require_endpoint_count(InfoResult &info_result, const std::size_t count) {
  REQUIRE(!info_result->get().endpoints_empty());
  REQUIRE(info_result->get().endpoints_size() == count);
}

template<typename OwnerType, typename TargetType>
auto require_endpoint(schema &repo) {
  auto info = require_info<OwnerType>(repo);
  auto it = info->get().find_relation_endpoint(std::type_index(typeid(TargetType)));
  REQUIRE(it != info->get().endpoint_end());
  REQUIRE(it->second);
  return it->second;
}

void require_linked_pair(const std::shared_ptr<matador::query::relation_endpoint> &left,
                         const std::shared_ptr<matador::query::relation_endpoint> &right) {
  REQUIRE(left);
  REQUIRE(right);
  REQUIRE(left->foreign_endpoint());
  REQUIRE(right->foreign_endpoint());
  REQUIRE(left->foreign_endpoint().get() == right.get());
  REQUIRE(right->foreign_endpoint().get() == left.get());
}

struct department;
struct employee;

struct department {
  unsigned int id{};
  std::string name;
  collection<object_ptr<employee>> employees;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, matador::VarChar63);
    field::has_many(op, "employees", employees, "department_id", matador::query::CascadeAllFetchLazy);
  }
};

struct employee {
  unsigned int id{};
  std::string name;
  object_ptr<department> department_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, matador::VarChar63);
    field::belongs_to(op, "department_id", department_, matador::query::CascadeAllFetchLazy);
  }
};

struct user;
struct session;

struct user {
  unsigned int id{};
  std::string name;
  object_ptr<session> session_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, matador::VarChar63);
    field::has_one(op, "session", session_, "user_id", matador::query::CascadeAllFetchLazy);
  }
};

struct session {
  unsigned int id{};
  std::string token;
  object_ptr<user> user_;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
    field::attribute(op, "token", token, matador::VarChar255);
    field::belongs_to(op, "user_id", user_, matador::query::CascadeAllFetchLazy);
  }
};

struct tag;

struct article {
  unsigned int id{};
  std::string title;
  collection<object_ptr<tag>> tags;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
    field::attribute(op, "title", title, matador::VarChar255);
    field::has_many(op, "article_tags", tags, "article_id", matador::query::CascadeAllFetchLazy);
  }
};

struct tag {
  unsigned int id{};
  std::string name;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, matador::VarChar63);
  }
};

struct account {
  unsigned int id{};
  std::string name;
  collection<std::string> roles;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, matador::VarChar63);
    field::has_many(op, "roles", roles, "account_id", matador::query::CascadeAllFetchLazy);
  }
};

struct recipe;
struct ingredient;

struct recipe {
  unsigned int id{};
  std::string name;
  collection<object_ptr<ingredient>> ingredients;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, matador::VarChar255);
    field::has_many_to_many(op, "recipe_ingredients", ingredients, "recipe_id", "ingredient_id", matador::query::CascadeAllFetchLazy);
  }
};

struct ingredient {
  unsigned int id{};
  std::string name;
  collection<object_ptr<recipe>> recipes;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, matador::VarChar255);
    field::has_many_to_many(op, "recipe_ingredients", recipes, "ingredient_id", "recipe_id", matador::query::CascadeAllFetchLazy);
  }
};

struct invalid_has_many_side;
struct invalid_has_one_side;

struct invalid_has_many_side {
  unsigned int id{};
  collection<object_ptr<invalid_has_one_side>> children;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
    field::has_many(op, "children", children, "parent_id", matador::query::CascadeAllFetchLazy);
  }
};

struct invalid_has_one_side {
  unsigned int id{};
  object_ptr<invalid_has_many_side> parent;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
    field::has_one(op, "parent", parent, "parent_id", matador::query::CascadeAllFetchLazy);
  }
};

struct multi_parent_a;
struct multi_parent_b;
struct multi_child;

struct multi_parent_a {
  unsigned int id{};
  collection<object_ptr<multi_child>> children;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
    field::has_many(op, "children", children, "parent_a_id", matador::query::CascadeAllFetchLazy);
  }
};

struct multi_parent_b {
  unsigned int id{};

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);
  }
};

struct multi_child {
  unsigned int id{};
  object_ptr<multi_parent_b> parent_b;
  object_ptr<multi_parent_a> parent_a;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = matador::query::field;
    field::primary_key(op, "id", id);

    // Absichtlich zuerst der nicht passende belongs_to-Endpunkt.
    // Das deckt die Regression ab, dass join_column_finder einen bereits
    // gefundenen Treffer nicht wieder auf false zurücksetzen darf.
    field::belongs_to(op, "parent_b_id", parent_b, matador::query::CascadeAllFetchLazy);
    field::belongs_to(op, "parent_a_id", parent_a, matador::query::CascadeAllFetchLazy);
  }
};

}

TEST_CASE("relation_completer links has_many to belongs_to", "[relation_completer][has_many][belongs_to]") {
  schema repo;

  auto result = repo.attach<department>("departments")
    .and_then([&repo] {
      return repo.attach<employee>("employees");
    });

  REQUIRE(result.is_ok());
  REQUIRE(repo.size() == 2);
  REQUIRE(repo.contains("departments"));
  REQUIRE(repo.contains("employees"));

  auto department_info = require_info<department>(repo);
  auto employee_info = require_info<employee>(repo);

  require_endpoint_count(department_info, 1);
  require_endpoint_count(employee_info, 1);

  auto department_endpoint = require_endpoint<department, employee>(repo);
  auto employee_endpoint = require_endpoint<employee, department>(repo);

  REQUIRE(department_endpoint->type() == relation_type::HasMany);
  REQUIRE(employee_endpoint->type() == relation_type::BelongsTo);
  REQUIRE(department_endpoint->field_name() == "employees");
  REQUIRE(employee_endpoint->field_name() == "department_id");

  require_linked_pair(department_endpoint, employee_endpoint);
}

TEST_CASE("relation_completer links has_many to belongs_to with reversed attach", "[relation_completer][has_many][belongs_to][reverse]") {
  schema repo;

  auto result = repo.attach<employee>("employees")
    .and_then([&repo] {
      return repo.attach<department>("departments");
    });

  REQUIRE(result.is_ok());
  REQUIRE(repo.size() == 2);
  REQUIRE(repo.contains("departments"));
  REQUIRE(repo.contains("employees"));

  auto department_info = require_info<department>(repo);
  auto employee_info = require_info<employee>(repo);

  require_endpoint_count(department_info, 1);
  require_endpoint_count(employee_info, 1);

  auto department_endpoint = require_endpoint<department, employee>(repo);
  auto employee_endpoint = require_endpoint<employee, department>(repo);

  REQUIRE(department_endpoint->type() == relation_type::HasMany);
  REQUIRE(employee_endpoint->type() == relation_type::BelongsTo);

  require_linked_pair(department_endpoint, employee_endpoint);
}

TEST_CASE("relation_completer links has_one to belongs_to", "[relation_completer][has_one][belongs_to]") {
  schema repo;

  auto result = repo.attach<user>("users")
    .and_then([&repo] {
      return repo.attach<session>("sessions");
    });

  REQUIRE(result.is_ok());
  REQUIRE(repo.size() == 2);
  REQUIRE(repo.contains("users"));
  REQUIRE(repo.contains("sessions"));

  auto user_info = require_info<user>(repo);
  auto session_info = require_info<session>(repo);

  require_endpoint_count(user_info, 1);
  require_endpoint_count(session_info, 1);

  auto user_endpoint = require_endpoint<user, session>(repo);
  auto session_endpoint = require_endpoint<session, user>(repo);

  REQUIRE(user_endpoint->type() == relation_type::HasOne);
  REQUIRE(session_endpoint->type() == relation_type::BelongsTo);
  REQUIRE(user_endpoint->field_name() == "session");
  REQUIRE(session_endpoint->field_name() == "user_id");

  require_linked_pair(user_endpoint, session_endpoint);
}

TEST_CASE("relation_completer links has_one to belongs_to with reversed attach order", "[relation_completer][has_one][belongs_to][reverse]") {
  schema repo;

  auto result = repo.attach<session>("sessions")
    .and_then([&repo] {
      return repo.attach<user>("users");
    });

  REQUIRE(result.is_ok());
  REQUIRE(repo.size() == 2);
  REQUIRE(repo.contains("users"));
  REQUIRE(repo.contains("sessions"));

  auto user_info = require_info<user>(repo);
  auto session_info = require_info<session>(repo);

  require_endpoint_count(user_info, 1);
  require_endpoint_count(session_info, 1);

  auto user_endpoint = require_endpoint<user, session>(repo);
  auto session_endpoint = require_endpoint<session, user>(repo);

  REQUIRE(user_endpoint->type() == relation_type::HasOne);
  REQUIRE(session_endpoint->type() == relation_type::BelongsTo);

  require_linked_pair(user_endpoint, session_endpoint);
}

TEST_CASE("relation_completer creates relation table for unidirectional has_many object", "[relation_completer][has_many][relation_table]") {
  schema repo;

  auto result = repo.attach<tag>("tags")
    .and_then([&repo] {
      return repo.attach<article>("articles");
    });

  REQUIRE(result.is_ok());

  REQUIRE(repo.contains("tags"));
  REQUIRE(repo.contains("articles"));
  REQUIRE(repo.contains("tags"));

  // Der aktuelle relation_completer verwendet den Feldnamen als Namen der
  // automatisch erzeugten Relationstabelle.
  REQUIRE(repo.contains("tags"));

  auto article_info = require_info<article>(repo);
  require_endpoint_count(article_info, 1);

  auto article_endpoint = require_endpoint<article, tag>(repo);
  REQUIRE(article_endpoint->type() == relation_type::HasMany);
  REQUIRE(article_endpoint->field_name() == "article_tags");
}

TEST_CASE("relation_completer creates relation table for has_many with value type", "[relation_completer][has_many][value_type]") {
  schema repo;

  auto result = repo.attach<account>("accounts");

  REQUIRE(result.is_ok());
  REQUIRE(repo.contains("accounts"));
  REQUIRE(repo.contains("roles"));

  auto account_info = require_info<account>(repo);
  auto roles_info = require_info(repo, "roles");

  require_endpoint_count(account_info, 1);
  require_endpoint_count(roles_info, 1);

  auto account_endpoint = account_info->get().find_relation_endpoint(std::type_index(typeid(std::string)));
  REQUIRE(account_endpoint != account_info->get().endpoint_end());
  REQUIRE(account_endpoint->second);
  REQUIRE(account_endpoint->second->type() == relation_type::HasMany);
  REQUIRE(account_endpoint->second->field_name() == "roles");

  auto roles_endpoint = roles_info->get().find_relation_endpoint(std::type_index(typeid(account)));
  REQUIRE(roles_endpoint != roles_info->get().endpoint_end());
  REQUIRE(roles_endpoint->second);
  REQUIRE(roles_endpoint->second->type() == relation_type::BelongsTo);
  REQUIRE(roles_endpoint->second->field_name() == "account_id");

  require_linked_pair(account_endpoint->second, roles_endpoint->second);
}

TEST_CASE("relation_completer creates many_to_many relation table", "[relation_completer][many_to_many]") {
  schema repo;

  auto result = repo.attach<recipe>("recipes")
    .and_then([&repo] {
      return repo.attach<ingredient>("ingredients");
    });

  REQUIRE(result.is_ok());

  REQUIRE(repo.contains("recipes"));
  REQUIRE(repo.contains("ingredients"));
  REQUIRE(repo.contains("recipe_ingredients"));
  REQUIRE(repo.size() == 3);

  auto recipe_info = require_info<recipe>(repo);
  auto ingredient_info = require_info<ingredient>(repo);
  auto relation_info = require_info(repo, "recipe_ingredients");

  require_endpoint_count(recipe_info, 1);
  require_endpoint_count(ingredient_info, 1);
  require_endpoint_count(relation_info, 2);

  auto recipe_endpoint = require_endpoint<recipe, ingredient>(repo);
  auto ingredient_endpoint = require_endpoint<ingredient, recipe>(repo);

  REQUIRE(recipe_endpoint->type() == relation_type::HasMany);
  REQUIRE(ingredient_endpoint->type() == relation_type::HasMany);
  REQUIRE(recipe_endpoint->field_name() == "recipe_ingredients");
  REQUIRE(ingredient_endpoint->field_name() == "recipe_ingredients");

  REQUIRE(recipe_endpoint->foreign_endpoint());
  REQUIRE(ingredient_endpoint->foreign_endpoint());

  REQUIRE(recipe_endpoint->foreign_endpoint()->type() == relation_type::BelongsTo);
  REQUIRE(ingredient_endpoint->foreign_endpoint()->type() == relation_type::BelongsTo);
}

TEST_CASE("relation_completer creates many_to_many relation table at reversed attach", "[relation_completer][many_to_many][reverse]") {
  schema repo;

  auto result = repo.attach<ingredient>("ingredients")
    .and_then([&repo] {
      return repo.attach<recipe>("recipes");
    });

  REQUIRE(result.is_ok());

  REQUIRE(repo.contains("recipes"));
  REQUIRE(repo.contains("ingredients"));
  REQUIRE(repo.contains("recipe_ingredients"));
  REQUIRE(repo.size() == 3);

  auto recipe_info = require_info<recipe>(repo);
  auto ingredient_info = require_info<ingredient>(repo);
  auto relation_info = require_info(repo, "recipe_ingredients");

  require_endpoint_count(recipe_info, 1);
  require_endpoint_count(ingredient_info, 1);
  require_endpoint_count(relation_info, 2);
}

TEST_CASE("relation_completer denies has_many to has_one as incompatible relation", "[relation_completer][invalid][has_many][has_one]") {
  schema repo;

  auto result = repo.attach<invalid_has_many_side>("invalid_many")
    .and_then([&repo] {
      return repo.attach<invalid_has_one_side>("invalid_one");
    });

  REQUIRE(result.is_error());
  REQUIRE(result.err() == matador::query::error_code::InvalidArgument);
}

TEST_CASE("relation_completer join_column_finder keeps already found hits", "[relation_completer][regression][join_column_finder]") {
  schema repo;

  auto result = repo.attach<multi_parent_b>("multi_parent_b")
    .and_then([&repo] {
      return repo.attach<multi_child>("multi_children");
    })
    .and_then([&repo] {
      return repo.attach<multi_parent_a>("multi_parent_a");
    });

  REQUIRE(result.is_ok());

  REQUIRE(repo.contains("multi_parent_a"));
  REQUIRE(repo.contains("multi_parent_b"));
  REQUIRE(repo.contains("multi_children"));

  auto parent_a_info = require_info<multi_parent_a>(repo);
  auto child_info = require_info<multi_child>(repo);

  require_endpoint_count(parent_a_info, 1);
  REQUIRE(child_info->get().endpoints_size() == 2);

  auto parent_a_endpoint = require_endpoint<multi_parent_a, multi_child>(repo);
  auto child_to_parent_a_endpoint = require_endpoint<multi_child, multi_parent_a>(repo);

  REQUIRE(parent_a_endpoint->type() == relation_type::HasMany);
  REQUIRE(child_to_parent_a_endpoint->type() == relation_type::BelongsTo);

  require_linked_pair(parent_a_endpoint, child_to_parent_a_endpoint);
}