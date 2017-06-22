//
// Created by sascha on 6/9/17.
//

#include "matador/object/object_store.hpp"

#include "../Item.hpp"

#include "RelationTestUnit.hpp"

RelationTestUnit::RelationTestUnit()
  : unit_test("relation", "Relation Test Unit")
{
  add_test("has_one", std::bind(&RelationTestUnit::test_has_one, this), "test has one relation");
  add_test("belongs_to_one", std::bind(&RelationTestUnit::test_belongs_to_one, this), "test belongs to one relation");
  add_test("has_many", std::bind(&RelationTestUnit::test_has_many, this), "test has many relation");
  add_test("has_many_builtin", std::bind(&RelationTestUnit::test_has_many_builtin, this), "test has many relation with builtin");
  add_test("belongs_to_many", std::bind(&RelationTestUnit::test_belongs_to_many, this), "test belongs to many relation");
  add_test("has_many_to_many", std::bind(&RelationTestUnit::test_has_many_to_many, this), "test has many to many relation");
}

void RelationTestUnit::test_has_one()
{
  std::cout << "\n";

  matador::object_store store;

  store.attach<child>("child");
  store.attach<master>("master");

  UNIT_ASSERT_EQUAL(2UL, store.size(), "must be two nodes");

  for (auto &node : store) {
    std::cout << "\n";
    for (auto &endpoint : node.endpoints()) {
      std::cout << "node [" << node.type() << "] has endpoint: " << endpoint.second->field << " (type: " << endpoint.second->type_name << ")\n";
    }
  }

  auto chld = store.insert(new child("child 1"));

  std::cout << "detail: " << chld->name << "\n";

//  chld = nullptr;

  auto mstr = store.insert(new master("m1", chld));

  std::cout << "detail: " << mstr->children->name << "\n";

  mstr->children = nullptr;
}

void RelationTestUnit::test_belongs_to_one()
{
  std::cout << "\n";

  matador::object_store store;

  store.attach<person>("person");
  store.attach<citizen, person>("citizen");
  store.attach<address>("address");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  for (auto &node : store) {
    std::cout << "\n";
    for (auto &endpoint : node.endpoints()) {
      std::cout << "node [" << node.type() << "] has endpoint: " << endpoint.second->field << " (type: " << endpoint.second->type_name << ")\n";
    }
  }

  auto george = store.insert(new citizen("george"));
  auto home = store.insert(new address("foreststreet", "foresting"));

  std::cout << "george id " << george.id() << "\n";
  std::cout << "home id " << home.id() << "\n";
  std::cout << "home->citizen: " << &home->citizen_ << " (" << home->citizen_.id() << ")\n";

  george->address_ = home;

  std::cout << "george->address: " << george->address_->street << "\n";
  std::cout << "home->citizen: " << home->citizen_->name() << " (" << home->citizen_.id() << ")\n";

  george->address_ = nullptr;

  std::cout << "george->address: " << &george->address_ << "\n";
  std::cout << "home->citizen: " << &home->citizen_ << " (" << home->citizen_.id() << ")\n";

}

void RelationTestUnit::test_has_many()
{
  std::cout << "\n";

  matador::object_store store;

  store.attach<child>("child");
  store.attach<children_vector>("children_vector");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  for (auto &node : store) {
    std::cout << "\n";
    for (auto &endpoint : node.endpoints()) {
      std::cout << "node [" << node.type() << "] has endpoint: " << endpoint.second->field << " (type: " << endpoint.second->type_name << ")\n";
    }
  }
}

void RelationTestUnit::test_has_many_builtin()
{

}

void RelationTestUnit::test_belongs_to_many()
{
  std::cout << "\n";

  matador::object_store store;

  store.attach<person>("person");
  store.attach<employee, person>("employee");
  store.attach<department>("department");

  UNIT_ASSERT_EQUAL(3UL, store.size(), "must be three nodes");

  for (auto &node : store) {
    std::cout << "\n";
    for (auto &endpoint : node.endpoints()) {
      std::cout << "node [" << node.type() << "] has endpoint: " << endpoint.second->field << " (type: " << endpoint.second->type_name << ")\n";
    }
  }

  auto jane = store.insert(new employee("jane"));
  auto insurance = store.insert(new department("insurance"));

  jane->dep(insurance);

  std::cout << "[" << &insurance->employees << "] dep size: " << insurance->employees.size() << "\n";

  jane->dep(nullptr);

  std::cout << "[" << &insurance->employees << "] dep size: " << insurance->employees.size() << "\n";

  insurance->employees.push_back(jane);

  std::cout << "[" << &insurance->employees << "] dep size: " << insurance->employees.size() << "\n";

  insurance->employees.clear();

  std::cout << "[" << &insurance->employees << "] dep size: " << insurance->employees.size() << "\n";
}

void RelationTestUnit::test_has_many_to_many()
{
  std::cout << "\n";

  matador::object_store store;

  store.attach<person>("person");
  store.attach<student, person>("student");
  store.attach<course>("course");

  UNIT_ASSERT_EQUAL(4UL, store.size(), "must be four nodes");

  for (auto &node : store) {
    std::cout << "\n";
    for (auto &endpoint : node.endpoints()) {
      std::cout << "node [" << node.type() << "/" << endpoint.first.name() << "] has endpoint: " << endpoint.second->field << " (type: " << endpoint.second->type_name << ")\n";
      std::cout << "endpoint " << typeid(*endpoint.second).name() << "\n";
    }
  }

  auto jane = store.insert(new student("jane"));
  auto art = store.insert(new course("art"));

  std::cout << "jane id " << jane.id() << "\n";
  std::cout << "art id " << art.id() << "\n";

  jane->courses.push_back(art); // jane (value) must be push_back to course art (owner) students!!

  std::cout << "[" << &art->students << "] students size: " << art->students.size() << "\n";
}
