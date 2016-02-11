//
// Created by sascha on 9/7/15.
//

#include "SQLTestUnit.hpp"

#include "../Item.hpp"

#include "sql/statement.hpp"
#include "sql/query.hpp"

using namespace oos;

SQLTestUnit::SQLTestUnit(const std::string &name, const std::string &msg, const std::string &db)
  : unit_test(name, msg)
  , db_(db)
{
  add_test("create", std::bind(&SQLTestUnit::test_create, this), "test direct sql create statement");
  add_test("statement", std::bind(&SQLTestUnit::test_statement, this), "test prepared sql statement");
  add_test("foreign_query", std::bind(&SQLTestUnit::test_foreign_query, this), "test query with foreign key");
  add_test("query", std::bind(&SQLTestUnit::test_query, this), "test query");
}

SQLTestUnit::~SQLTestUnit() {}

void SQLTestUnit::initialize()
{
  connection_.reset(create_connection());
}

void SQLTestUnit::finalize() { }


void SQLTestUnit::test_create()
{
  connection_->open();

  query<Item> q(*connection_, "item");

  result<Item> res(q.create().execute());

  auto itime = oos::time(2015, 3, 15, 13, 56, 23, 123);
  Item hans("Hans", 4711);
  hans.set_time(itime);
  res = q.insert(&hans).execute();

  res = q.select().execute();

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<Item> item(first.release());
    UNIT_ASSERT_EQUAL(item->get_string(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->get_int(), 4711, "expected integer must be 4711");
    UNIT_ASSERT_EQUAL(item->get_time(), itime, "expected time is invalid");
    ++first;
  }

  res = q.drop().execute();
}

void SQLTestUnit::test_statement()
{
  connection_->open();

  query<Item> q(*connection_, "item");

  statement<Item> stmt(q.create().prepare());

  result<Item> res(stmt.execute());

  auto itime = oos::time(2015, 3, 15, 13, 56, 23, 123);
  Item hans("Hans", 4711);
  hans.set_time(itime);
  stmt = q.insert(&hans).prepare();

  stmt.bind(&hans);
  res = stmt.execute();

  stmt = q.select().prepare();
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

  stmt = q.drop().prepare();

  res = stmt.execute();
}

void SQLTestUnit::test_foreign_query()
{
  connection_->open();

  query<Item> q(*connection_, "item");

  using t_object_item = ObjectItem<Item>;

  // create item table and insert item
  result<Item> res(q.create().execute());

  auto itime = oos::time(2015, 3, 15, 13, 56, 23, 123);
  oos::identifier<unsigned long> id(23);
  Item *hans = new Item("Hans", 4711);
  hans->id(id.value());
  hans->set_time(itime);
  res = q.insert(hans).execute();

  query<t_object_item> object_item_query(*connection_, "object_item");
  result<t_object_item> ores = object_item_query.create().execute();

  t_object_item oitem;
  oitem.ptr(hans);

  ores = object_item_query.insert(&oitem).execute();

  ores = object_item_query.select().execute();

  auto first = ores.begin();
  auto last = ores.end();

  while (first != last) {
    std::unique_ptr<t_object_item> obj(first.release());

    object_ptr<Item> i = obj->ptr();
    UNIT_ASSERT_TRUE(i.has_primary_key(), "expected valid identifier");
    UNIT_ASSERT_TRUE(*i.primary_key() == id, "expected id must be 23");

    ++first;
  }

  object_item_query.drop().execute();

  q.drop().execute();
}

void SQLTestUnit::test_query()
{
  connection_->open();

  query<person> q(*connection_, "person");

  // create item table and insert item
  result<person> res(q.create().execute());

  person hans("Hans", oos::date(12, 3, 1980), 180);
  hans.id(1);
  res = q.insert(&hans).execute();

  res = q.select().where("name='hans'").execute();

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());

    UNIT_ASSERT_EQUAL(item->name(), "Hans", "expected name must be 'Hans'");
    UNIT_ASSERT_EQUAL(item->height(), 180U, "expected height must be 180");
    UNIT_ASSERT_EQUAL(item->birthdate(), oos::date(12, 3, 1980), "expected birthdate is 12.3.1980");

    ++first;
  }

  res = q.select().where("name='heinz'").execute();

  UNIT_ASSERT_TRUE(res.begin() == res.end(), "begin must be equal end");

  q.drop().execute();
}

connection* SQLTestUnit::create_connection()
{
  return new connection(db_);
}

std::string SQLTestUnit::db() const {
  return db_;
}
