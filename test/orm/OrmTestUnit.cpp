//
// Created by sascha on 20.03.16.
//

#include "OrmTestUnit.hpp"

#include "../Item.hpp"

#include "orm/persistence.hpp"
#include "orm/session.hpp"

OrmTestUnit::OrmTestUnit(const std::string &prefix, const std::string &dns)
  : unit_test(prefix + "_orm", prefix + " orm test unit")
  , dns_(dns)
{
  add_test("create", std::bind(&OrmTestUnit::test_create, this), "test orm create table");
  add_test("insert", std::bind(&OrmTestUnit::test_insert, this), "test orm insert into table");
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

  std::unique_ptr<person> p1 = res.begin().release();

  UNIT_EXPECT_EQUAL("hans", p1->name(), "invalid name");
  
  p.drop();
}
