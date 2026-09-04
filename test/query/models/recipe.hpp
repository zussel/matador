#ifndef QUERY_RECIPE_HPP
#define QUERY_RECIPE_HPP

#include "matador/query/access.hpp"
#include "matador/query/foreign_options.hpp"
#include "matador/query/object_ptr.hpp"
#include "matador/query/collection.hpp"

#include <string>
#include <utility>

namespace matador::test {
struct recipe;

struct ingredient {
  unsigned int id{};
  std::string name;
  query::collection<query::object_ptr<recipe> > recipes{};

  ingredient() = default;

  ingredient(const unsigned int id, std::string name)
  : id(id), name(std::move(name)) {
  }

  template<class Operator>
  void process(Operator &op) {
    query::field::primary_key(op, "id", id);
    query::field::attribute(op, "name", name, UniqueVarChar255);
    query::field::has_many_to_many(op, "recipe_ingredients", recipes, "ingredient_id", "recipe_id", CascadeAllFetchEager);
  }
};

struct recipe {
  unsigned int id{};
  std::string name;
  query::collection<query::object_ptr<ingredient> > ingredients{};

  recipe() = default;
  recipe(const unsigned int id, std::string name)
  : id(id), name(std::move(name)) {
  }
  recipe(const unsigned int id, std::string name, std::vector<query::object_ptr<ingredient>> ings)
  : id(id)
  , name(std::move(name))
  , ingredients(std::move(ings)){
  }

  template<class Operator>
  void process(Operator &op) {
    query::field::primary_key(op, "id", id);
    query::field::attribute(op, "name", name, UniqueVarChar255);
    query::field::has_many_to_many(op, "recipe_ingredients", ingredients, CascadeAllFetchLazy);
  }
};

template<const query::primary_key_options &PkAttribute>
struct recipe_pk_generator;

template<const query::primary_key_options &PkAttribute>
struct ingredient_pk_generator {
  unsigned int id{};
  std::string name;
  query::collection<query::object_ptr<recipe_pk_generator<PkAttribute>>> recipes{};

  ingredient_pk_generator() = default;

  explicit ingredient_pk_generator(std::string name)
  : name(std::move(name)) {
  }

  ingredient_pk_generator(std::string name, std::vector<query::object_ptr<recipe_pk_generator<PkAttribute>>> recps)
  : name(std::move(name))
  , recipes(std::move(recps)){}

  template<class Operator>
  void process(Operator &op) {
    query::field::primary_key(op, "id", id, PkAttribute);
    query::field::attribute(op, "name", name, UniqueVarChar255);
    query::field::has_many_to_many(op, "recipe_ingredients", recipes, "ingredient_id", "recipe_id", CascadeAllFetchEager);
  }
};

template<const query::primary_key_options &PkAttribute>
struct recipe_pk_generator {
  unsigned int id{};
  std::string name;
  query::collection<query::object_ptr<ingredient_pk_generator<PkAttribute>>> ingredients{};

  recipe_pk_generator() = default;
  explicit recipe_pk_generator(std::string name)
  : name(std::move(name)) {
  }
  recipe_pk_generator(std::string name, std::vector<query::object_ptr<ingredient_pk_generator<PkAttribute>>> ings)
  : name(std::move(name))
  , ingredients(std::move(ings)){}

  template<class Operator>
  void process(Operator &op) {
    query::field::primary_key(op, "id", id, PkAttribute);
    query::field::attribute(op, "name", name, UniqueVarChar255);
    query::field::has_many_to_many(op, "recipe_ingredients", ingredients, CascadeAllFetchLazy);
  }
};

using ingredient_identity = ingredient_pk_generator<query::Identity>;
using ingredient_table = ingredient_pk_generator<query::Table>;
using ingredient_sequence = ingredient_pk_generator<query::Sequence>;
using recipe_identity = recipe_pk_generator<query::Identity>;
using recipe_table = recipe_pk_generator<query::Table>;
using recipe_sequence = recipe_pk_generator<query::Sequence>;
}

#endif //QUERY_RECIPE_HPP
