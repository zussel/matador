#ifndef QUERY_RECIPE_HPP
#define QUERY_RECIPE_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/foreign_attributes.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/container.hpp"
#include "matador/object/has_many_to_many_item.hpp"

//#include "matador/sql/has_many_to_many_relation.hpp"

#include <string>
#include <vector>

namespace matador::test {

struct recipe;
struct ingredient
{
  unsigned long id{};
  std::string name;
  container<recipe> recipes{};

  template<class Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_many_to_many(op, "recipe_ingredients", recipes, "ingredient_id", "recipe_id", utils::fetch_type::EAGER);
  }
};

struct recipe
{
  unsigned long id{};
  std::string name;
  container<ingredient> ingredients{};

  template<class Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, 255);
    field::has_many_to_many(op, "recipe_ingredients", ingredients, utils::fetch_type::LAZY);
  }
};

class recipe_ingredient : public has_many_to_many_item<recipe, ingredient>
{
public:
  recipe_ingredient()
  : has_many_to_many_item("recipe_id", "ingredient_id") {}
};

}

#endif //QUERY_RECIPE_HPP
