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
  add_test("save_object", std::bind(&OrmRelationTestUnit::test_save_object, this), "test save object");
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
    varchars.modify()->elements.push_back("george");
    varchars.modify()->elements.push_back("jane");
    varchars.modify()->elements.push_back("william");
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
  std::cout << "\n";

  matador::persistence p(dns_);

  p.enable_log();

  p.attach<many_list_ints>("many_ints");

  p.create();

  matador::session s(p);

  auto ints = s.insert(new many_list_ints);

  UNIT_ASSERT_GREATER(ints->id, 0UL);
  UNIT_ASSERT_TRUE(ints->elements.empty());

  s.push_back(ints.modify()->elements, 37);
  s.push_back(ints.modify()->elements, 4711);
  s.push_back(ints.modify()->elements, -12345);
  s.push_back(ints.modify()->elements, 37);

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

  s.push_back(children.modify()->children, kid1);
  s.push_back(children.modify()->children, kid1);
  s.push_back(children.modify()->children, kid2);

  UNIT_ASSERT_FALSE(children->children.empty());
  UNIT_ASSERT_EQUAL(children->children.size(), 3UL);

  s.erase(children.modify()->children, children.modify()->children.begin());

  UNIT_ASSERT_FALSE(children->children.empty());
  UNIT_ASSERT_EQUAL(children->children.size(), 2UL);

  s.erase(children.modify()->children, children.modify()->children.begin(), children.modify()->children.end());

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
  s.push_back(dep.modify()->employees, george);

  UNIT_ASSERT_EQUAL(dep->employees.size(), 1UL);
  UNIT_ASSERT_EQUAL(dep->employees.front()->name(), "george");
  UNIT_ASSERT_FALSE(george->dep().empty());
  UNIT_ASSERT_EQUAL(george->dep()->name, dep->name);

  // jane is automatically added to deps employee list
  jane.modify()->dep(dep);
  s.update(jane);

  UNIT_ASSERT_EQUAL(dep->employees.size(), 2UL);

  // remove george
  s.erase(dep.modify()->employees, dep.modify()->employees.begin());

  UNIT_ASSERT_EQUAL(dep->employees.size(), 1UL);
  UNIT_ASSERT_TRUE(george->dep().empty());
  UNIT_ASSERT_EQUAL(dep->employees.front()->name(), "jane");
//
  jane.modify()->department_.clear();
  s.update(jane);
//
  UNIT_ASSERT_TRUE(dep->employees.empty());

  p.drop();
}

void OrmRelationTestUnit::test_many_to_many()
{
  matador::persistence p(dns_);

  p.attach_abstract<person>("person");
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

  s.push_back(art.modify()->students, jane);

  UNIT_ASSERT_FALSE(art->students.empty());
  UNIT_ASSERT_EQUAL(art->students.size(), 1UL);
  UNIT_ASSERT_EQUAL(art->students.front()->name(), jane->name());
  UNIT_ASSERT_FALSE(jane->courses.empty());
  UNIT_ASSERT_EQUAL(jane->courses.size(), 1UL);
  UNIT_ASSERT_EQUAL(jane->courses.front()->title, art->title);

  s.erase(jane.modify()->courses, jane.modify()->courses.begin());

  UNIT_ASSERT_TRUE(jane->courses.empty());
  UNIT_ASSERT_TRUE(art->students.empty());

  s.push_back(george.modify()->courses, algebra);

  UNIT_ASSERT_FALSE(algebra->students.empty());
  UNIT_ASSERT_EQUAL(algebra->students.size(), 1UL);
  UNIT_ASSERT_EQUAL(algebra->students.front()->name(), george->name());
  UNIT_ASSERT_FALSE(george->courses.empty());
  UNIT_ASSERT_EQUAL(george->courses.size(), 1UL);
  UNIT_ASSERT_EQUAL(george->courses.front()->title, algebra->title);

  s.clear(algebra.modify()->students);

  UNIT_ASSERT_TRUE(george->courses.empty());
  UNIT_ASSERT_TRUE(algebra->students.empty());

  p.drop();
}

void OrmRelationTestUnit::test_save()
{
  std::cout << "\n";

  matador::persistence p(dns_);

  p.enable_log();

  p.attach<many_list_ints>("many_ints");

  p.create();

  matador::session s(p);

  auto ints = s.insert_only(new many_list_ints);

  UNIT_ASSERT_GREATER(ints->id, 0UL);
  UNIT_ASSERT_TRUE(ints->elements.empty());

  ints.modify()->elements.push_back(37);
  ints.modify()->elements.push_back(4711);
  ints.modify()->elements.push_back(-12345);
  ints.modify()->elements.push_back(37);

  s.flush();
//  s.save(ints);

  UNIT_ASSERT_FALSE(ints->elements.empty());
  UNIT_ASSERT_EQUAL(ints->elements.size(), 4UL);

  p.drop();
}

void OrmRelationTestUnit::test_save_object()
{
  std::cout << "\n";

  matador::persistence p(dns_);

  p.enable_log();

  p.attach<child>("child");
  p.attach<children_list>("children_list");

  p.create();

  matador::session s(p);

  auto children = s.insert_only(new children_list("children list"));
  auto kid1 = s.insert_only(new child("kid 1"));
  auto kid2 = s.insert_only(new child("kid 2"));

  UNIT_ASSERT_GREATER(children->id, 0UL);
  UNIT_ASSERT_TRUE(children->children.empty());

  children.modify()->children.push_back(kid1);
  children.modify()->children.push_back(kid2);

  children.modify()->children.erase(children.modify()->children.begin());

  s.remove_only(kid1);

  s.flush();

//  children.modify()->children.erase(children->children.begin());
//
//  s.remove_only(kid2);
//
//  s.flush();

//  s.save(ints);

  UNIT_EXPECT_FALSE(children->children.empty());
  UNIT_EXPECT_EQUAL(children->children.size(), 1UL);

  p.drop();
}
