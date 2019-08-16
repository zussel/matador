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
using namespace matador;

OrmTestUnit::OrmTestUnit(const std::string &prefix, std::string dns)
  : unit_test(prefix + "_orm", prefix + " orm test unit")
  , dns_(std::move(dns))
{
  add_test("persistence", std::bind(&OrmTestUnit::test_persistence, this), "test persistence class");
  add_test("table", std::bind(&OrmTestUnit::test_table, this), "test table class");
  add_test("create", std::bind(&OrmTestUnit::test_create, this), "test create table");
  add_test("insert", std::bind(&OrmTestUnit::test_insert, this), "test insert into table");
  add_test("select", std::bind(&OrmTestUnit::test_select, this), "test select a table");
  add_test("update", std::bind(&OrmTestUnit::test_update, this), "test update on table");
  add_test("delete", std::bind(&OrmTestUnit::test_delete, this), "test delete from table");
  add_test("save", std::bind(&OrmTestUnit::test_save, this), "test session save");
  add_test("flush", std::bind(&OrmTestUnit::test_flush, this), "test session flush");
}

void OrmTestUnit::test_persistence()
{
  matador::persistence p(dns_);

  UNIT_EXPECT_FALSE(p.is_log_enabled());

  p.enable_log();

  UNIT_EXPECT_TRUE(p.is_log_enabled());

  p.disable_log();

  UNIT_EXPECT_FALSE(p.is_log_enabled());

  p.attach<person>("person");

  for (auto const &tbl : p) {
    UNIT_EXPECT_EQUAL("person", tbl.second->name());
  }

  UNIT_EXPECT_EQUAL(1UL, p.store().size());

  UNIT_ASSERT_TRUE(p.conn().is_connected());

  p.detach("person");

  UNIT_EXPECT_TRUE(p.end() == p.find_table("person"));
}

void OrmTestUnit::test_table()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  auto tbl = p.find_table("person");

  UNIT_EXPECT_FALSE(p.end() == p.find_table("person"));

  UNIT_EXPECT_EQUAL("person", tbl->second->node().type());

  UNIT_EXPECT_EQUAL(&p, &tbl->second->persistence_unit());

  p.create();

  matador::session s(p);

  auto hans = s.insert(new person("hans", matador::date(18, 5, 1980), 180));

  UNIT_EXPECT_GREATER(hans->id(), 0UL);

  UNIT_ASSERT_TRUE(is_loaded(hans));

  p.drop();
}

void OrmTestUnit::test_create()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  // check if table exists
  UNIT_EXPECT_TRUE(p.exists<person>());

  p.drop();

  // check if table exists
  UNIT_EXPECT_FALSE(p.exists<person>());
}

void OrmTestUnit::test_insert()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  matador::session s(p);

  auto hans = s.insert(new person("hans", matador::date(18, 5, 1980), 180));

  UNIT_EXPECT_GREATER(hans->id(), 0UL);

  matador::query<person> q("person");
  auto res = q.select().where(matador::column("name") == "hans").execute(p.conn());

  auto first = res.begin();

  UNIT_ASSERT_TRUE(first != res.end());

  std::unique_ptr<person> p1(first.release());

  UNIT_EXPECT_EQUAL("hans", p1->name());

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

  for (auto const &name : names) {
    auto pptr = s.insert(new person(name, matador::date(18, 5, 1980), 180));
  	UNIT_EXPECT_GREATER(pptr->id(), 0UL);
  }

  auto view = s.select<person>();

  UNIT_ASSERT_EQUAL(view.size(), names.size());

  for (auto optr : view) {
    UNIT_ASSERT_TRUE(contains(names, optr->name()));
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

  UNIT_EXPECT_GREATER(hans->id(), 0UL);
  UNIT_EXPECT_EQUAL(hans->height(), 180U);
  UNIT_EXPECT_EQUAL(hans->birthdate(), birthday);

  hans->height(179);

  hans = s.update(hans);

  UNIT_EXPECT_EQUAL(hans->height(), 179U);

  matador::query<person> q("person");
  matador::connection c(dns_);
  c.connect();
  auto res = q.select().where(matador::column("name") == "hans").execute(c);

  auto first = res.begin();

  UNIT_ASSERT_TRUE(first != res.end());

  std::unique_ptr<person> p1(first.release());

  UNIT_EXPECT_EQUAL("hans", p1->name());
  UNIT_EXPECT_EQUAL(179U, p1->height());
  UNIT_EXPECT_EQUAL(hans->birthdate(), birthday);

  p.drop();
}

void OrmTestUnit::test_delete()
{
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  matador::session s(p);

  auto hans = s.insert(new person("hans", matador::date(18, 5, 1980), 180));

  UNIT_EXPECT_GREATER(hans->id(), 0UL);

  matador::query<person> q("person");
  matador::connection c(dns_);
  c.connect();
  auto res = q.select().where(matador::column("name") == "hans").execute(c);

  auto first = res.begin();

  UNIT_ASSERT_TRUE(first != res.end());

  std::unique_ptr<person> p1((first++).release());

  UNIT_EXPECT_EQUAL("hans", p1->name());

  s.remove(hans);

  res = q.select().where(matador::column("name") == "hans").execute(c);

  first = res.begin();

  UNIT_EXPECT_TRUE(first == res.end());
//  UNIT_EXPECT_TRUE(res.empty());

  p.drop();
}

void OrmTestUnit::test_save() {
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  matador::session s(p);

  matador::date birthday(18, 5, 1980);
  auto hans = s.save(new person("hans", birthday, 180));

  UNIT_EXPECT_GREATER(hans->id(), 0UL);
  UNIT_EXPECT_EQUAL(hans->height(), 180U);
  UNIT_EXPECT_EQUAL(hans->birthdate(), birthday);

  hans->height(179);

  hans = s.save(hans);

  UNIT_EXPECT_EQUAL(hans->height(), 179U);

  matador::connection conn(dns_);
  conn.connect();

  matador::query<person> q("person");
  auto res = q.select().where("name"_col == "hans").execute(conn);

  auto first = res.begin();

  UNIT_ASSERT_TRUE(first != res.end());

  std::unique_ptr<person> p1(first.release());

  UNIT_EXPECT_EQUAL("hans", p1->name());
  UNIT_EXPECT_EQUAL(179U, p1->height());
  UNIT_EXPECT_EQUAL(hans->birthdate(), birthday);

  s.remove(hans);

  res = q.select().where("name"_col == "hans").execute(conn);

  first = res.begin();

  UNIT_EXPECT_TRUE(first == res.end());

  p.drop();
}

void OrmTestUnit::test_flush()
{
  std::cout << "\n";
  matador::persistence p(dns_);

  p.attach<person>("person");

  p.create();

  matador::session s(p);

  matador::date birthday(18, 5, 1980);
  auto hans = s.insert_only(new person("hans", birthday, 180));

  UNIT_EXPECT_GREATER(hans->id(), 0UL);
  UNIT_EXPECT_EQUAL(hans->height(), 180U);
  UNIT_EXPECT_EQUAL(hans->birthdate(), birthday);

  hans->height(179);

  auto george = s.insert_only(new person("george", birthday, 154));

  UNIT_EXPECT_GREATER(george->id(), 0UL);
  UNIT_EXPECT_EQUAL(george->height(), 154U);
  UNIT_EXPECT_EQUAL(george->birthdate(), birthday);

  george->height(153);

  s.flush();

//  UNIT_EXPECT_EQUAL(hans->height(), 179U);
//
//  matador::connection conn(dns_);
//  conn.connect();
//
//  matador::query<person> q("person");
//  auto res = q.select().where("name"_col == "hans").execute(conn);
//
//  auto first = res.begin();
//
//  UNIT_ASSERT_TRUE(first != res.end());
//
//  std::unique_ptr<person> p1(first.release());
//
//  UNIT_EXPECT_EQUAL("hans", p1->name());
//  UNIT_EXPECT_EQUAL(179U, p1->height());
//  UNIT_EXPECT_EQUAL(hans->birthdate(), birthday);
//
//  s.remove(hans);
//
//  res = q.select().where("name"_col == "hans").execute(conn);
//
//  first = res.begin();
//
//  UNIT_EXPECT_TRUE(first == res.end());

  p.drop();
}
