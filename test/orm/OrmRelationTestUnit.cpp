//
// Created by sascha on 16.03.17.
//

#include "OrmRelationTestUnit.hpp"

#include "../Item.hpp"
#include "../entities.hpp"
#include "../has_many_list.hpp"

#include "matador/orm/persistence.hpp"
#include "matador/orm/session.hpp"

#include "matador/object/object_view.hpp"

using namespace hasmanylist;

OrmRelationTestUnit::OrmRelationTestUnit(const std::string &prefix, std::string dns)
  : unit_test(prefix + "_orm_relation", prefix + " orm relation test unit")
  , dns_(std::move(dns))
{
  add_test("has_many_builtin_varchars", std::bind(&OrmRelationTestUnit::test_has_builtin_varchars, this), "test has many builtin varchars item");
  add_test("has_many_builtin_ints", std::bind(&OrmRelationTestUnit::test_has_builtin_ints, this), "test has many builtin ints item");
  add_test("has_many_delete", std::bind(&OrmRelationTestUnit::test_has_many_delete, this), "test has many delete item");
  add_test("belongs_to", std::bind(&OrmRelationTestUnit::test_belongs_to, this), "test belongs to");
  add_test("has_many_to_many", std::bind(&OrmRelationTestUnit::test_many_to_many, this), "test has many to many");
  add_test("save", std::bind(&OrmRelationTestUnit::test_save, this), "test save");
}

using many_list_varchars = many_builtins<matador::varchar<255>, std::list>;

void OrmRelationTestUnit::test_has_builtin_varchars()
{
  matador::persistence p(dns_);

  p.attach<many_list_varchars>("many_varchars");

  p.create();

  matador::session s(p);

  auto varchars = s.insert(new many_list_varchars);

  UNIT_ASSERT_GREATER(varchars->id, 0UL);
  UNIT_ASSERT_TRUE(varchars->elements.empty());

  auto tr = s.begin();

  try {
    varchars->elements.push_back("george");
    varchars->elements.push_back("jane");
    varchars->elements.push_back("william");
    tr.commit();
  } catch (std::exception &) {
    tr.rollback();
  }

  UNIT_ASSERT_FALSE(varchars->elements.empty());
  UNIT_ASSERT_EQUAL(varchars->elements.size(), 3UL);

  p.drop();
}

using many_list_ints = many_builtins<int, std::list>;

void OrmRelationTestUnit::test_has_builtin_ints()
{
  matador::persistence p(dns_);

  p.attach<many_list_ints>("many_ints");

  p.create();

  matador::session s(p);

  auto ints = s.insert(new many_list_ints);

  UNIT_ASSERT_GREATER(ints->id, 0UL);
  UNIT_ASSERT_TRUE(ints->elements.empty());

  s.push_back(ints->elements, 37);
  s.push_back(ints->elements, 4711);
  s.push_back(ints->elements, -12345);
  s.push_back(ints->elements, 37);

  UNIT_ASSERT_FALSE(ints->elements.empty());
  UNIT_ASSERT_EQUAL(ints->elements.size(), 4UL);

  p.drop();
}

void OrmRelationTestUnit::test_has_many_delete()
{
  matador::persistence p(dns_);

  p.attach<child>("child");
  p.attach<children_list>("children_list");

  p.create();

  matador::session s(p);

  auto children = s.insert(new children_list("children list"));

  UNIT_ASSERT_GREATER(children->id, 0UL);
  UNIT_ASSERT_TRUE(children->children.empty());

  auto kid1 = s.insert(new child("kid 1"));
  auto kid2 = s.insert(new child("kid 2"));

  UNIT_ASSERT_GREATER(kid1->id, 0UL);
  UNIT_ASSERT_GREATER(kid2->id, 0UL);

  s.push_back(children->children, kid1);
  s.push_back(children->children, kid1);
  s.push_back(children->children, kid2);

  UNIT_ASSERT_FALSE(children->children.empty());
  UNIT_ASSERT_EQUAL(children->children.size(), 3UL);

  s.erase(children->children, children->children.begin());

  UNIT_ASSERT_FALSE(children->children.empty());
  UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

  s.erase(children->children, children->children.begin(), children->children.end());

  UNIT_ASSERT_GREATER(children->id, 0UL);
  UNIT_ASSERT_TRUE(children->children.empty());

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

  UNIT_ASSERT_TRUE(dep->employees.empty());
  UNIT_ASSERT_TRUE(george->dep().empty());
  UNIT_ASSERT_TRUE(jane->dep().empty());

  // department is automatically set
  s.push_back(dep->employees, george);

  UNIT_ASSERT_EQUAL(dep->employees.size(), 1UL);
  UNIT_ASSERT_EQUAL(dep->employees.front()->name(), "george");
  UNIT_ASSERT_FALSE(george->dep().empty());
  UNIT_ASSERT_EQUAL(george->dep()->name, dep->name);

  // jane is automatically added to deps employee list
  jane->dep(dep);
  s.update(jane);

  UNIT_ASSERT_EQUAL(dep->employees.size(), 2UL);

  // remove george
  s.erase(dep->employees, dep->employees.begin());

  UNIT_ASSERT_EQUAL(dep->employees.size(), 1UL);
  UNIT_ASSERT_TRUE(george->dep().empty());
  UNIT_ASSERT_EQUAL(dep->employees.front()->name(), "jane");
//
  jane->department_.clear();
  s.update(jane);
//
  UNIT_ASSERT_TRUE(dep->employees.empty());

  p.drop();
}

void OrmRelationTestUnit::test_many_to_many()
{
  matador::persistence p(dns_);

  p.attach<person>("person", matador::object_store::abstract_type::abstract);
  p.attach<student, person>("student");
  p.attach<course>("course");

  UNIT_ASSERT_EQUAL(4UL, p.store().size());

  p.create();

  matador::session s(p);

  auto george = s.insert(new student("george"));
  auto jane = s.insert(new student("jane"));
  auto algebra = s.insert(new course("algebra"));
  auto art = s.insert(new course("art"));

  UNIT_ASSERT_TRUE(george->courses.empty());
  UNIT_ASSERT_TRUE(jane->courses.empty());
  UNIT_ASSERT_TRUE(algebra->students.empty());
  UNIT_ASSERT_TRUE(art->students.empty());

  s.push_back(art->students, jane);

  UNIT_ASSERT_FALSE(art->students.empty());
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
  UNIT_ASSERT_EQUAL(art->students.front()->name(), jane->name());
  UNIT_ASSERT_FALSE(jane->courses.empty());
  UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL);
  UNIT_ASSERT_EQUAL(jane->courses.front()->title, art->title);

  s.erase(jane->courses, jane->courses.begin());

  UNIT_ASSERT_TRUE(jane->courses.empty());
  UNIT_ASSERT_TRUE(art->students.empty());

  s.push_back(george->courses, algebra);

  UNIT_ASSERT_FALSE(algebra->students.empty());
  UNIT_ASSERT_EQUAL(algebra->students.size(), 1UL);
  UNIT_ASSERT_EQUAL(algebra->students.front()->name(), george->name());
  UNIT_ASSERT_FALSE(george->courses.empty());
  UNIT_ASSERT_EQUAL(george->courses.size(), 1UL);
  UNIT_ASSERT_EQUAL(george->courses.front()->title, algebra->title);

  s.clear(algebra->students);

  UNIT_ASSERT_TRUE(george->courses.empty());
  UNIT_ASSERT_TRUE(algebra->students.empty());

  p.drop();
}

void OrmRelationTestUnit::test_save()
{
  matador::persistence p(dns_);

  p.attach<many_list_ints>("many_ints");

  p.create();

  matador::session s(p);

  auto ints = s.save(new many_list_ints);

  UNIT_ASSERT_GREATER(ints->id, 0UL);
  UNIT_ASSERT_TRUE(ints->elements.empty());

  ints->elements.push_back(37);
  ints->elements.push_back(4711);
  ints->elements.push_back(-12345);
  ints->elements.push_back(37);

  s.save(ints);

  UNIT_ASSERT_FALSE(ints->elements.empty());
  UNIT_ASSERT_EQUAL(ints->elements.size(), 4UL);

  p.drop();
}
