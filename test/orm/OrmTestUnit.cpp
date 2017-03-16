//
// Created by sascha on 20.03.16.
//

#include "OrmTestUnit.hpp"

#include "../Item.hpp"
#include "../has_many_list.hpp"

#include "matador/orm/persistence.hpp"
#include "matador/orm/session.hpp"

#include "matador/object/object_view.hpp"

using namespace hasmanylist;

OrmTestUnit::OrmTestUnit(const std::string &prefix, const std::string &dns)
  : unit_test(prefix + "_orm", prefix + " orm test unit")
  , dns_(dns)
{
  add_test("create", std::bind(&OrmTestUnit::test_create, this), "test create table");
  add_test("insert", std::bind(&OrmTestUnit::test_insert, this), "test insert into table");
  add_test("select", std::bind(&OrmTestUnit::test_select, this), "test select a table");
  add_test("update", std::bind(&OrmTestUnit::test_update, this), "test update on table");
  add_test("delete", std::bind(&OrmTestUnit::test_delete, this), "test delete from table");
  add_test("load", std::bind(&OrmTestUnit::test_load, this), "test load from table");
  add_test("load_has_one", std::bind(&OrmTestUnit::test_load_has_one, this), "test load has one relation from table");
  add_test("load_has_many", std::bind(&OrmTestUnit::test_load_has_many, this), "test load has many from table");
  add_test("load_has_many_int", std::bind(&OrmTestUnit::test_load_has_many_int, this), "test load has many int from table");
  add_test("has_many_delete", std::bind(&OrmTestUnit::test_has_many_delete, this), "test has many delete item");
  add_test("belongs_to", std::bind(&OrmTestUnit::test_belongs_to, this), "test belongs to");
  add_test("many_to_many", std::bind(&OrmTestUnit::test_many_to_many, this), "test many to many");
}

void OrmTestUnit::test_create()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  // check if table exists
  UNIT_EXPECT_TRUE(p.exists<person>(), "table must exist");

  p.drop();

  // check if table exists
  UNIT_EXPECT_FALSE(p.exists<person>(), "table must not exist");
}

void OrmTestUnit::test_insert()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  matador::session s(p);

  auto hans = s.insert(new person("hans", matador::date(18, 5, 1980), 180));

  UNIT_EXPECT_GREATER(hans->id(), 0UL, "is must be greater zero");

  matador::query<person> q("person");
  auto res = q.select().where(matador::column("name") == "hans").execute(p.conn());

  auto first = res.begin();

  UNIT_ASSERT_TRUE(first != res.end(), "first must not end");

  std::unique_ptr<person> p1(first.release());

  UNIT_EXPECT_EQUAL("hans", p1->name(), "invalid name");

  // Todo: fix open cursor with Free TDS
  ++first;

  p.drop();
}

template < class C, class T >
bool contains(const C &container, const T &value)
{
  return std::find(std::begin(container), std::end(container), value) != std::end(container);
}

void OrmTestUnit::test_select()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  matador::session s(p);

  std::vector<std::string> names({ "hans", "otto", "georg", "hilde" });
  //std::vector<std::string> names({ "hans", "otto", "georg", "hilde", "ute", "manfred" });

  for (std::string name : names) {
    auto pptr = s.insert(new person(name, matador::date(18, 5, 1980), 180));
  	UNIT_EXPECT_GREATER(pptr->id(), 0UL, "is must be greater zero");
  }

  auto view = s.select<person>();

  UNIT_ASSERT_EQUAL(view.size(), names.size(), "unexpected size");

  for (auto optr : view) {
    UNIT_ASSERT_TRUE(contains(names, optr->name()), "unknown name from view");
  }

  p.drop();
}

void OrmTestUnit::test_update()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  matador::session s(p);

  matador::date birthday(18, 5, 1980);
  auto hans = s.insert(new person("hans", birthday, 180));

  UNIT_EXPECT_GREATER(hans->id(), 0UL, "id must be greater zero");
  UNIT_EXPECT_EQUAL(hans->height(), 180U, "height must be 180");
  UNIT_EXPECT_EQUAL(hans->birthdate(), birthday, "birthday must be equal");

  hans->height(179);

  hans = s.update(hans);

  UNIT_EXPECT_EQUAL(hans->height(), 179U, "height must be 179");

  matador::query<person> q("person");
  matador::connection c(dns_);
  c.open();
  auto res = q.select().where(matador::column("name") == "hans").execute(c);

  auto first = res.begin();

  UNIT_ASSERT_TRUE(first != res.end(), "first must not end");

  std::unique_ptr<person> p1(first.release());

  UNIT_EXPECT_EQUAL("hans", p1->name(), "invalid name");
  UNIT_EXPECT_EQUAL(179U, p1->height(), "height must be 179");
  UNIT_EXPECT_EQUAL(hans->birthdate(), birthday, "birthday must be equal");

  p.drop();
}

void OrmTestUnit::test_delete()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  matador::session s(p);

  auto hans = s.insert(new person("hans", matador::date(18, 5, 1980), 180));

  UNIT_EXPECT_GREATER(hans->id(), 0UL, "is must be greater zero");

  matador::query<person> q("person");
  matador::connection c(dns_);
  c.open();
  auto res = q.select().where(matador::column("name") == "hans").execute(c);

  auto first = res.begin();

  UNIT_ASSERT_TRUE(first != res.end(), "first must not end");

  std::unique_ptr<person> p1((first++).release());

  UNIT_EXPECT_EQUAL("hans", p1->name(), "invalid name");

  s.remove(hans);

  res = q.select().where(matador::column("name") == "hans").execute(c);

  first = res.begin();

  UNIT_EXPECT_TRUE(first == res.end(), "first must be end");
//  UNIT_EXPECT_TRUE(res.empty(), "result set must be empty");

  p.drop();
}

void OrmTestUnit::test_load()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  std::vector<std::string> names({"hans", "otto", "georg", "hilde", "ute", "manfred"});

  {
    // insert some persons
    matador::session s(p);

    for (std::string name : names) {
      auto pptr = s.insert(new person(name, matador::date(18, 5, 1980), 180));

      UNIT_EXPECT_GREATER(pptr->id(), 0UL, "is must be greater zero");
    }
  }

  p.clear();

  {
    // load persons from database
    matador::session s(p);

    s.load();

    typedef matador::object_view<person> t_person_view;
    t_person_view persons(s.store());

    UNIT_ASSERT_TRUE(!persons.empty(), "person view must not be empty");
    UNIT_ASSERT_EQUAL(persons.size(), 6UL, "thier must be 6 persons");

    t_person_view::iterator first = persons.begin();
    t_person_view::iterator last = persons.end();

    while (first != last) {
      auto pptr = *first++;
      names.erase(std::remove_if(std::begin(names), std::end(names), [pptr](const std::string &name) {
        return name == pptr->name();
      }), names.end());
    }
    UNIT_ASSERT_TRUE(names.empty(), "names must be empty");
  }

  p.drop();
}

void OrmTestUnit::test_load_has_one()
{
  matador::persistence p(dns_);

  p.attach<master>("master");
  p.attach<child>("child");

  p.create();

  {
    // insert some persons
    matador::session s(p);

    auto c = s.insert(new child("child 1"));

    auto m = new master("master 1");
    m->children = c;
    s.insert(m);
  }

  p.clear();

  {
    // load persons from database
    matador::session s(p);

    s.load();

    typedef matador::object_view<master> t_master_view;
    t_master_view masters(s.store());

    UNIT_ASSERT_TRUE(!masters.empty(), "master view must not be empty");
    UNIT_ASSERT_EQUAL(masters.size(), 1UL, "their must be 1 master");

    auto mptr = masters.front();
    UNIT_ASSERT_NOT_NULL(mptr->children.get(), "child must be valid");
  }

  p.drop();
}

void OrmTestUnit::test_load_has_many()
{
  matador::persistence p(dns_);

  p.attach<child>("child");
  p.attach<children_list>("children_list");

  p.create();

  {
    matador::session s(p);

    auto children = s.insert(new children_list("children list 1"));

    UNIT_ASSERT_GREATER(children->id, 0UL, "invalid children list");
    UNIT_ASSERT_TRUE(children->children.empty(), "children list must be empty");

    auto kid1 = s.insert(new child("kid 1"));
    auto kid2 = s.insert(new child("kid 2"));

    UNIT_ASSERT_GREATER(kid1->id, 0UL, "invalid child");
    UNIT_ASSERT_GREATER(kid2->id, 0UL, "invalid child");

    s.push_back(children->children, kid1);
    s.push_back(children->children, kid2);

    UNIT_ASSERT_FALSE(children->children.empty(), "children list couldn't be empty");
    UNIT_ASSERT_EQUAL(children->children.size(), 2UL, "invalid children list size");
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    typedef matador::object_view<children_list> t_children_list_view;
    t_children_list_view children_lists(s.store());

    UNIT_ASSERT_TRUE(!children_lists.empty(), "children lists view must not be empty");
    UNIT_ASSERT_EQUAL(children_lists.size(), 1UL, "their must be 1 children list");

    auto clptr = children_lists.front();

    UNIT_ASSERT_FALSE(clptr->children.empty(), "children list couldn't be empty");
    UNIT_ASSERT_EQUAL(clptr->children.size(), 2UL, "invalid children list size");

    std::vector<std::string> result_names({ "kid 1", "kid 2"});
    for (auto kid : clptr->children) {
      //std::cout << "kid " << kid->name << " (id: " << kid->id << ")\n";
      auto it = std::find(result_names.begin(), result_names.end(), kid->name);
      UNIT_EXPECT_FALSE(it == result_names.end(), "kid must be found");
    }
  }

  p.drop();
}

void OrmTestUnit::test_load_has_many_int()
{
  matador::persistence p(dns_);

  p.attach<many_ints>("many_ints");

  p.create();

  {
    matador::session s(p);

    auto intlist = s.insert(new many_ints);

    UNIT_ASSERT_GREATER(intlist->id, 0UL, "invalid intlist list");
    UNIT_ASSERT_TRUE(intlist->elements.empty(), "intlist list must be empty");

    s.push_back(intlist->elements, 4);

    UNIT_ASSERT_EQUAL(intlist->elements.front(), 4, "first int must be 4");
    UNIT_ASSERT_EQUAL(intlist->elements.back(), 4, "last int must be 4");

    s.push_front(intlist->elements, 7);

    UNIT_ASSERT_EQUAL(intlist->elements.front(), 7, "first int must be 7");

    UNIT_ASSERT_FALSE(intlist->elements.empty(), "intlist list couldn't be empty");
    UNIT_ASSERT_EQUAL(intlist->elements.size(), 2UL, "invalid intlist list size");
  }

  p.clear();

  {
    matador::session s(p);

    s.load();

    typedef matador::object_view<many_ints> t_many_ints_view;
    t_many_ints_view ints_view(s.store());

    UNIT_ASSERT_TRUE(!ints_view.empty(), "many ints view must not be empty");
    UNIT_ASSERT_EQUAL(ints_view.size(), 1UL, "their must be 1 int in many ints list");

    auto intlist = ints_view.front();

    UNIT_ASSERT_FALSE(intlist->elements.empty(), "intlist list couldn't be empty");
    UNIT_ASSERT_EQUAL(intlist->elements.size(), 2UL, "invalid intlist list size");

    std::vector<int> result_ints({ 4, 7 });
    for (auto i : intlist->elements) {
      auto it = std::find(result_ints.begin(), result_ints.end(), i);
      UNIT_EXPECT_FALSE(it == result_ints.end(), "int must be found");
    }

  }

  p.drop();
}

void OrmTestUnit::test_has_many_delete()
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

void OrmTestUnit::test_belongs_to()
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

void OrmTestUnit::test_many_to_many()
{
  matador::persistence p(dns_);

  p.attach<person>("person");
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
