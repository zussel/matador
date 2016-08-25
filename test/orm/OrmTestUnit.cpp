//
// Created by sascha on 20.03.16.
//

#include "OrmTestUnit.hpp"

#include "../Item.hpp"

#include "orm/persistence.hpp"
#include "orm/session.hpp"

#include "object/object_view.hpp"

OrmTestUnit::OrmTestUnit(const std::string &prefix, const std::string &dns)
  : unit_test(prefix + "_orm", prefix + " orm test unit")
  , dns_(dns)
{
  add_test("create", std::bind(&OrmTestUnit::test_create, this), "test orm create table");
  add_test("insert", std::bind(&OrmTestUnit::test_insert, this), "test orm insert into table");
  add_test("update", std::bind(&OrmTestUnit::test_update, this), "test orm update on table");
  add_test("delete", std::bind(&OrmTestUnit::test_delete, this), "test orm delete from table");
  add_test("load", std::bind(&OrmTestUnit::test_load, this), "test orm load from table");
  add_test("load_has_one", std::bind(&OrmTestUnit::test_load_has_one, this), "test orm load has one relation from table");
  add_test("has_many_delete", std::bind(&OrmTestUnit::test_has_many_delete, this), "test orm has many delete item");
}

void OrmTestUnit::test_create()
{
  oos::persistence p(dns_);

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
  oos::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  oos::session s(p);

  auto hans = s.insert(new person("hans", oos::date(18, 5, 1980), 180));

  UNIT_EXPECT_GREATER(hans->id(), 0UL, "is must be greater zero");

  oos::query<person> q("person");
  oos::connection c(dns_);
  c.open();
  auto res = q.select().where(oos::column("name") == "hans").execute(c);

  auto first = res.begin();

  UNIT_ASSERT_TRUE(first != res.end(), "first must not end");

  std::unique_ptr<person> p1(first.release());

  UNIT_EXPECT_EQUAL("hans", p1->name(), "invalid name");
  
  p.drop();
}

void OrmTestUnit::test_update()
{
  oos::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  oos::session s(p);

  oos::date birthday(18, 5, 1980);
  auto hans = s.insert(new person("hans", birthday, 180));

  UNIT_EXPECT_GREATER(hans->id(), 0UL, "id must be greater zero");
  UNIT_EXPECT_EQUAL(hans->height(), 180U, "height must be 180");
  UNIT_EXPECT_EQUAL(hans->birthdate(), birthday, "birthday must be equal");

  hans->height(179);

  hans = s.update(hans);

  UNIT_EXPECT_EQUAL(hans->height(), 179U, "height must be 179");

  oos::query<person> q("person");
  oos::connection c(dns_);
  c.open();
  auto res = q.select().where(oos::column("name") == "hans").execute(c);

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
  oos::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  oos::session s(p);

  auto hans = s.insert(new person("hans", oos::date(18, 5, 1980), 180));

  UNIT_EXPECT_GREATER(hans->id(), 0UL, "is must be greater zero");

  oos::query<person> q("person");
  oos::connection c(dns_);
  c.open();
  auto res = q.select().where(oos::column("name") == "hans").execute(c);

  auto first = res.begin();

  UNIT_ASSERT_TRUE(first != res.end(), "first must not end");

  std::unique_ptr<person> p1((first++).release());

  UNIT_EXPECT_EQUAL("hans", p1->name(), "invalid name");

  s.remove(hans);

  res = q.select().where(oos::column("name") == "hans").execute(c);

  first = res.begin();

  UNIT_EXPECT_TRUE(first == res.end(), "first must be end");
//  UNIT_EXPECT_TRUE(res.empty(), "result set must be empty");

  p.drop();
}

void OrmTestUnit::test_load()
{
  oos::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  std::vector<std::string> names({"hans", "otto", "georg", "hilde", "ute", "manfred"});

  {
    // insert some persons
    oos::session s(p);

    for (std::string name : names) {
      auto pptr = s.insert(new person(name, oos::date(18, 5, 1980), 180));

      UNIT_EXPECT_GREATER(pptr->id(), 0UL, "is must be greater zero");
    }
  }

  p.clear();

  {
    // load persons from database
    oos::session s(p);

    s.load();

    typedef oos::object_view<person> t_person_view;
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
  oos::persistence p(dns_);

  p.attach<master>("master");
  p.attach<child>("child");

  p.create();

  {
    // insert some persons
    oos::session s(p);

    auto c = s.insert(new child("child 1"));

    auto m = new master("master 1");
    m->children = c;
    s.insert(m);
  }

  p.clear();

  {
    // load persons from database
    oos::session s(p);

    s.load();

    typedef oos::object_view<master> t_master_view;
    t_master_view masters(s.store());

    UNIT_ASSERT_TRUE(!masters.empty(), "master view must not be empty");
    UNIT_ASSERT_EQUAL(masters.size(), 1UL, "their must be 1 master");

    auto mptr = masters.front();
    UNIT_ASSERT_NOT_NULL(mptr->children.get(), "child must be valid");
  }

  p.drop();
}

void OrmTestUnit::test_has_many_delete()
{
  oos::persistence p(dns_);

  p.attach<child>("child");
  p.attach<children_list>("children_list");

  p.create();

  oos::session s(p);

  auto children = s.insert(new children_list("children list"));

  UNIT_ASSERT_GREATER(children->id, 0UL, "invalid children list");
  UNIT_ASSERT_TRUE(children->children.empty(), "children list must be empty");

  auto kid1 = s.insert(new child("kid 1"));
  auto kid2 = s.insert(new child("kid 2"));

  UNIT_ASSERT_GREATER(kid1->id, 0UL, "invalid child");
  UNIT_ASSERT_GREATER(kid2->id, 0UL, "invalid child");

  children->children.push_back(kid1);
  children->children.push_back(kid2);

  UNIT_ASSERT_FALSE(children->children.empty(), "children list couldn't be empty");
  UNIT_ASSERT_EQUAL(children->children.size(), 2UL, "invalid children list size");

  p.drop();
}
