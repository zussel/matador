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

  std::unique_ptr<person> p1(first.release());

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
      auto p = s.insert(new person(name, oos::date(18, 5, 1980), 180));

      UNIT_EXPECT_GREATER(p->id(), 0UL, "is must be greater zero");
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
      auto p = *first++;
      names.erase(std::remove_if(std::begin(names), std::end(names), [p](const std::string &name) {
        return name == p->name();
      }), names.end());
    }
    UNIT_ASSERT_TRUE(names.empty(), "names must be empty");
  }

  p.drop();
}
