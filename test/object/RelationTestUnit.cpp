//
// Created by sascha on 6/9/17.
//

#include "matador/object/object_store.hpp"
#include "matador/object/has_one.hpp"

#include "RelationTestUnit.hpp"

RelationTestUnit::RelationTestUnit()
  : unit_test("relation", "Relation Test Unit")
{
  add_test("has_one", std::bind(&RelationTestUnit::test_has_one, this), "test has one relation");
  add_test("has_many", std::bind(&RelationTestUnit::test_has_many, this), "test has many relation");
  add_test("has_many_builtin", std::bind(&RelationTestUnit::test_has_many_builtin, this), "test has many relation with builtin");
}

namespace relation {
struct detail
{
  std::string name;

  template<class S>
  void serialize(S &serializer)
  {
    serializer.serialize("name", name);
  }
};

struct master
{
  long id = 0;
  matador::has_one<detail> dtl;

  template<class S>
  void serialize(S &serializer)
  {
    serializer.serialize("id", id);
    serializer.serialize("detail", dtl, matador::cascade_type::ALL);
  }
};

}

void RelationTestUnit::test_has_one()
{
  matador::object_store store;

  store.attach<relation::detail>("detail");
  store.attach<relation::master>("master");


}

void RelationTestUnit::test_has_many()
{

}

void RelationTestUnit::test_has_many_builtin()
{

}
