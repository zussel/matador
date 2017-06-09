//
// Created by sascha on 6/9/17.
//

#include "RelationTestUnit.hpp"

RelationTestUnit::RelationTestUnit()
  : unit_test("relation", "Relation Test Unit")
{
  add_test("has_one", std::bind(&RelationTestUnit::test_has_one, this), "test has one relation");
  add_test("has_many", std::bind(&RelationTestUnit::test_has_many, this), "test has many relation");
  add_test("has_many_builtin", std::bind(&RelationTestUnit::test_has_many_builtin, this), "test has many relation with builtin");
}

void RelationTestUnit::test_has_one()
{

}

void RelationTestUnit::test_has_many()
{

}

void RelationTestUnit::test_has_many_builtin()
{

}
