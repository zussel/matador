//
// Created by sascha on 9/7/15.
//

#include "SQLTestUnit.hpp"

#include "../Item.hpp"

#include "database/session.hpp"
#include "database/query.hpp"
#include "database/result.hpp"
#include "database/statement.hpp"

using namespace oos;

SQLTestUnit::SQLTestUnit(const std::string &name, const std::string &msg, const std::string &db)
  : unit_test(name, msg)
  , db_(db)
{
    add_test("create", std::bind(&SQLTestUnit::test_create, this), "test direct sql create statement");
    add_test("statement", std::bind(&SQLTestUnit::test_statement, this), "test prepared sql statement");
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

//    query<Item> q(session_->db(), "item");
    query q(session_->db());

//    result<Item> res(q.create("item").execute());
    result res(q.create<Item>("item").execute());

    q.reset();

    Item hans("Hans", 4711);
    hans.set_time(oos::time(2015, 3, 15, 13, 56, 23, 123));
    res = q.insert(&hans, "item").execute();

    q.reset();

//    res = q.select().from("item").execute();
    res = q.select<Item>().from("item").execute();

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

//    query<Item> q(session_->db(), "item");
    query q(session_->db());

//    result<Item> res(q.create("item").execute());
    statement stmt(q.create<Item>("item").prepare());

    result res(stmt.execute());

    q.reset();

    Item hans("Hans", 4711);
    hans.set_time(oos::time(2015, 3, 15, 13, 56, 23, 123));
    stmt = q.insert(&hans, "item").prepare();

    stmt.bind(&hans);
    res = stmt.execute();

    q.reset();

    stmt = q.select<Item>().from("item").prepare();
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
