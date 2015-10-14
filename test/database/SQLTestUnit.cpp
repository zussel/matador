//
// Created by sascha on 9/7/15.
//

#include "SQLTestUnit.hpp"

#include "../Item.hpp"

#include "database/session.hpp"
#include "database/query.hpp"
#include "database/statement.hpp"

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

//    result<Item> res(q.create("item").execute());
  result res(q.create("item").execute());

  q.reset();

  Item hans("Hans", 4711);
  hans.set_time(oos::time(2015, 3, 15, 13, 56, 23, 123));
  res = q.insert(&hans, "item").execute();

  q.reset();

//    res = q.select().from("item").execute();
  res = q.select().from("item").execute();

//    result_iterator<Item> first = res.begin();
//    result_iterator<Item> last = res.end();
//    auto first = res.begin();
//    auto last = res.end();
  result_iterator first = res.begin();
  result_iterator last = res.end();

  while (first != last) {
//      Item *item = first.release();
//      delete item;
    serializable *obj = first.release();
    delete obj;
    ++first;
  }

  q.reset();

  res = q.drop("item").execute();
}

void SQLTestUnit::test_statement()
{
  session_->open();

  query<Item> q(session_->db());

//    result<Item> res(q.create("item").execute());
  statement stmt(q.create("item").prepare());

  result res(stmt.execute());

  q.reset();

  Item hans("Hans", 4711);
  hans.set_time(oos::time(2015, 3, 15, 13, 56, 23, 123));
  stmt = q.insert(&hans, "item").prepare();

  stmt.bind(&hans);
  res = stmt.execute();

  q.reset();

  stmt = q.select().from("item").prepare();
  res = stmt.execute();

//    result_iterator<Item> first = res.begin();
//    result_iterator<Item> last = res.end();
//    auto first = res.begin();
//    auto last = res.end();
  result_iterator first = res.begin();
  result_iterator last = res.end();

  while (first != last) {
//      Item *item = first.release();
//      delete item;
    serializable *obj = first.release();
    delete obj;
    ++first;
  }

  q.reset();

  stmt = q.drop("item").prepare();

  res = stmt.execute();
}

void SQLTestUnit::test_foreign_query()
{
  session_->open();

  query<Item> q(session_->db());

  using t_object_item = ObjectItem<Item>;

  // create item table and insert item
  result res(q.create("item").execute());
  q.reset();

  object_ptr<Item> hans(new Item("Hans", 4711));
  hans->id(23);
  hans->set_time(oos::time(2015, 3, 15, 13, 56, 23, 123));
  res = q.insert(hans, "item").execute();

  q.reset();

  query<t_object_item> object_item_query(session_->db());
//    result<Item> res(q.create("item").execute());
  res = object_item_query.create("object_item").execute();

  object_item_query.reset();

  t_object_item oitem;
  oitem.ptr(hans);

  res = object_item_query.insert(&oitem, "object_item").execute();

  object_item_query.reset();

  res = object_item_query.select().from("object_item").execute();

  result_iterator first = res.begin();
  result_iterator last = res.end();

  while (first != last) {
    serializable *obj = first.release();

    t_object_item *oi = dynamic_cast<t_object_item*>(obj);

    object_ptr<Item> i = oi->ptr();

    delete obj;
    ++first;
  }

  object_item_query.reset();

  object_item_query.drop("object_item").execute();

  q.reset();

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
