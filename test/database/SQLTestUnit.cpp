//
// Created by sascha on 9/7/15.
//

#include "SQLTestUnit.hpp"

#include "../Item.hpp"

#include "sql/statement.hpp"

#include "database/session.hpp"
#include "database/query.hpp"

using namespace oos;

SQLTestUnit::SQLTestUnit(const std::string &name, const std::string &msg, const std::string &db)
  : unit_test(name, msg)
  , db_(db)
{
  add_test("create", std::bind(&SQLTestUnit::test_create, this), "test direct sql create statement");
  add_test("statement", std::bind(&SQLTestUnit::test_statement, this), "test prepared sql statement");
  add_test("foreign_query", std::bind(&SQLTestUnit::test_foreign_query, this), "test query with foreign key");
}

SQLTestUnit::~SQLTestUnit() {}

void SQLTestUnit::initialize()
{
  session_.reset(create_session());
}

void SQLTestUnit::finalize() { }


void SQLTestUnit::test_create()
{
  session_->open();

  query<Item> q(session_->db());

  result<Item> res(q.create("item").execute());

  auto itime = oos::time(2015, 3, 15, 13, 56, 23, 123);
  Item hans("Hans", 4711);
  hans.set_time(itime);
  res = q.insert(&hans, "item").execute();

  res = q.select().from("item").execute();

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<Item> item(first.release());
    UNIT_ASSERT_EQUAL(item->get_string(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->get_int(), 4711, "expected integer must be 4711");
    UNIT_ASSERT_EQUAL(item->get_time(), itime, "expected time is invalid");
    ++first;
  }

  res = q.drop("item").execute();
}

void SQLTestUnit::test_statement()
{
  session_->open();

  query<Item> q(session_->db());

  statement<Item> stmt(q.create("item").prepare());

  result<Item> res(stmt.execute());

  auto itime = oos::time(2015, 3, 15, 13, 56, 23, 123);
  Item hans("Hans", 4711);
  hans.set_time(itime);
  stmt = q.insert(&hans, "item").prepare();

  stmt.bind(&hans);
  res = stmt.execute();

  stmt = q.select().from("item").prepare();
  res = stmt.execute();

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<Item> item(first.release());
    UNIT_ASSERT_EQUAL(item->get_string(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->get_int(), 4711, "expected integer must be 4711");
    UNIT_ASSERT_EQUAL(item->get_time(), itime, "expected time is invalid");
    ++first;
  }

  stmt = q.drop("item").prepare();

  res = stmt.execute();
}

void SQLTestUnit::test_foreign_query()
{
  session_->open();

  query<Item> q(session_->db());

  using t_object_item = ObjectItem<Item>;

  // create item table and insert item
  result<Item> res(q.create("item").execute());

  auto itime = oos::time(2015, 3, 15, 13, 56, 23, 123);
  oos::identifier<unsigned long> id(23);
  object_ptr<Item> hans(new Item("Hans", 4711));
  hans->id(id.value());
  hans->set_time(itime);
  res = q.insert(hans, "item").execute();

  query<t_object_item> object_item_query(session_->db());
  result<t_object_item> ores = object_item_query.create("object_item").execute();

  t_object_item oitem;
  oitem.ptr(hans);

  ores = object_item_query.insert(&oitem, "object_item").execute();

  ores = object_item_query.select().from("object_item").execute();

  auto first = ores.begin();
  auto last = ores.end();

  while (first != last) {
    std::unique_ptr<t_object_item> obj(first.release());

    object_ptr<Item> i = obj->ptr();
    UNIT_ASSERT_TRUE(i.has_primary_key(), "expected valid identifier");
    UNIT_ASSERT_TRUE(*i.primary_key() == id, "expected id must be 23");

    ++first;
  }

  object_item_query.drop("object_item").execute();

  q.drop("item").execute();
}

session* SQLTestUnit::create_session()
{
  return new session(ostore_, db_);
}

oos::object_store& SQLTestUnit::ostore()
{
  return ostore_;
}

const oos::object_store& SQLTestUnit::ostore() const
{
  return ostore_;
}

std::string SQLTestUnit::db() const {
  return db_;
}
