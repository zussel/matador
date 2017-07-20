//
// Created by sascha on 16.03.17.
//

#include "OrmRelationTestUnit.hpp"

#include "../Item.hpp"
#include "../has_many_list.hpp"

#include "matador/orm/persistence.hpp"
#include "matador/orm/session.hpp"

#include "matador/object/object_view.hpp"

using namespace hasmanylist;

OrmRelationTestUnit::OrmRelationTestUnit(const std::string &prefix, const std::string &dns)
  : unit_test(prefix + "_orm_relation", prefix + " orm relation test unit")
  , dns_(dns)
{
  add_test("has_many_delete", std::bind(&OrmRelationTestUnit::test_has_many_delete, this), "test has many delete item");
  add_test("belongs_to", std::bind(&OrmRelationTestUnit::test_belongs_to, this), "test belongs to");
  add_test("has_many_to_many", std::bind(&OrmRelationTestUnit::test_many_to_many, this), "test has many to many");
}

void OrmRelationTestUnit::test_has_many_delete()
{
  matador::persistence p(dns_);

  p.attach<child>("child");
  p.attach<children_list>("children_list");

  p.create();

  matador::session s(p);

  auto children = s.insert(new children_list("children list"));

  UNIT_ASSERT_GREATER(children->id, 0UL, "invalid children list");
  UNIT_ASSERT_TRUE(children->children.empty(), "children list must be empty");

  auto kid1 = s.insert(new child("kid 1"));
  auto kid2 = s.insert(new child("kid 2"));

  UNIT_ASSERT_GREATER(kid1->id, 0UL, "invalid child");
  UNIT_ASSERT_GREATER(kid2->id, 0UL, "invalid child");

  s.push_back(children->children, kid1);
  s.push_back(children->children, kid1);
  s.push_back(children->children, kid2);

  UNIT_ASSERT_FALSE(children->children.empty(), "children list couldn't be empty");
  UNIT_ASSERT_EQUAL(children->children.size(), 3UL, "invalid children list size");

  s.erase(children->children, children->children.begin());

  UNIT_ASSERT_FALSE(children->children.empty(), "children list couldn't be empty");
  UNIT_ASSERT_EQUAL(children->children.size(), 2UL, "invalid children list size");

  s.erase(children->children, children->children.begin(), children->children.end());

  UNIT_ASSERT_GREATER(children->id, 0UL, "invalid children list");
  UNIT_ASSERT_TRUE(children->children.empty(), "children list must be empty");

  p.drop();
}

void OrmRelationTestUnit::test_belongs_to()
{
  matador::persistence p(dns_);

  p.attach<person>("person");
  p.attach<department>("department");
  p.attach<employee, person>("employee");

  p.create();

  matador::session s(p);

  auto george = s.insert(new employee("george"));
  auto jane = s.insert(new employee("jane"));
  auto dep = s.insert(new department("insurance"));

  UNIT_ASSERT_TRUE(dep->employees.empty(), "there must be no employees");
  UNIT_ASSERT_TRUE(george->dep().empty(), "there must not be an department");
  UNIT_ASSERT_TRUE(jane->dep().empty(), "there must not be an department");

  // department is automatically set
  s.push_back(dep->employees, george);

  UNIT_ASSERT_EQUAL(dep->employees.size(), 1UL, "there must be one employee");
  UNIT_ASSERT_EQUAL(dep->employees.front()->name(), "george", "expected name must be george");
  UNIT_ASSERT_FALSE(george->dep().empty(), "department must not be empty");
  UNIT_ASSERT_EQUAL(george->dep()->name, dep->name, "names must be equal");

  // jane is automatically added to deps employee list
  jane->dep(dep);
  s.update(jane);

  UNIT_ASSERT_EQUAL(dep->employees.size(), 2UL, "there must be two employees");

  // remove george
  s.erase(dep->employees, dep->employees.begin());

  UNIT_ASSERT_EQUAL(dep->employees.size(), 1UL, "there must be one employee");
  UNIT_ASSERT_TRUE(george->dep().empty(), "there must not be an department");
  UNIT_ASSERT_EQUAL(dep->employees.front()->name(), "jane", "expected name must be jane");
//
  jane->department_.clear();
  s.update(jane);
//
  UNIT_ASSERT_TRUE(dep->employees.empty(), "there must be no employees");

  p.drop();
}

void OrmRelationTestUnit::test_many_to_many()
{
  matador::persistence p(dns_);

  p.attach<person>("person", matador::object_store::abstract_type::abstract);
  p.attach<student, person>("student");
  p.attach<course>("course");

  UNIT_ASSERT_EQUAL(4UL, p.store().size(), "unexpected size");

  p.create();

  matador::session s(p);

  auto george = s.insert(new student("george"));
  auto jane = s.insert(new student("jane"));
  auto algebra = s.insert(new course("algebra"));
  auto art = s.insert(new course("art"));

  UNIT_ASSERT_TRUE(george->courses.empty(), "georges courses must be empty");
  UNIT_ASSERT_TRUE(jane->courses.empty(), "janes courses must be empty");
  UNIT_ASSERT_TRUE(algebra->students.empty(), "there must be no students in algebra");
  UNIT_ASSERT_TRUE(art->students.empty(), "there must be no students in art");

  s.push_back(art->students, jane);

  UNIT_ASSERT_FALSE(art->students.empty(), "there must not be students in art");
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL, "there must be one student in art course");
  UNIT_ASSERT_EQUAL(art->students.front()->name(), jane->name(), "arts student must be jane");
  UNIT_ASSERT_FALSE(jane->courses.empty(), "janes courses must not be empty");
  UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL, "jane must've took one course");
  UNIT_ASSERT_EQUAL(jane->courses.front()->title, art->title, "janes course must be art");

  s.erase(jane->courses, jane->courses.begin());

  UNIT_ASSERT_TRUE(jane->courses.empty(), "janes courses must be empty");
  UNIT_ASSERT_TRUE(art->students.empty(), "there must be no students in art");

  s.push_back(george->courses, algebra);

  UNIT_ASSERT_FALSE(algebra->students.empty(), "there must not be students in algebra");
  UNIT_ASSERT_EQUAL(algebra->students.size(), 1UL, "there must be one student in algebra course");
  UNIT_ASSERT_EQUAL(algebra->students.front()->name(), george->name(), "algebras student must be george");
  UNIT_ASSERT_FALSE(george->courses.empty(), "georges courses must not be empty");
  UNIT_ASSERT_EQUAL(george->courses.size(), 1UL, "george must've took one course");
  UNIT_ASSERT_EQUAL(george->courses.front()->title, algebra->title, "georges course must be algebra");

  s.clear(algebra->students);

  UNIT_ASSERT_TRUE(george->courses.empty(), "georges courses must be empty");
  UNIT_ASSERT_TRUE(algebra->students.empty(), "there must be no students in algebra");

  p.drop();
}
