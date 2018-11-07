//
// Created by sascha on 20.03.16.
//

#include "OrmTestUnit.hpp"

#include "../Item.hpp"
#include "../person.hpp"
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
