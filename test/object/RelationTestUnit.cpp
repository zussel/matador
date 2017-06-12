//
// Created by sascha on 6/9/17.
//

#include "matador/object/object_store.hpp"
#include "matador/object/has_one.hpp"

#include "../Item.hpp"

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
  std::cout << "\n";

  matador::object_store store;

  store.attach<child>("child");
  auto node = store.attach<master>("master");

  for (auto endpoint : node->endpoints()) {
    std::cout << "node " << node->type() << " has endpoint: " << endpoint.second->field << "\n";
  }

  auto chld = store.insert(new child("child 1"));

  std::cout << "detail: " << chld->name << "\n";

  auto mstr = store.insert(new master("m1", chld));

  std::cout << "detail: " << mstr->children->name << "\n";
}

void RelationTestUnit::test_has_many()
{
  std::cout << "\n";

  matador::object_store store;

  store.attach<child>("child");
  auto node = store.attach<children_vector>("children_vector");

  std::cout << "\n";
  for (auto endpoint : node->endpoints()) {
    std::cout << "node " << node->type() << " has endpoint: " << endpoint.second->field << "\n";
  }
}

void RelationTestUnit::test_has_many_builtin()
{

}
