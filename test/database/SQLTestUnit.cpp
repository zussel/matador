//
// Created by sascha on 9/7/15.
//

#include "SQLTestUnit.hpp"

#include "../Item.hpp"

#include "database/session.hpp"
#include "database/query.hpp"
#include "database/result.hpp"

using namespace oos;

SQLTestUnit::SQLTestUnit(const std::string &name, const std::string &msg, const std::string &db)
  : unit_test(name, msg)
  , db_(db)
{
    add_test("create", std::bind(&SQLTestUnit::test_create, this), "test direct sql create statement");
}

SQLTestUnit::~SQLTestUnit() {}

void SQLTestUnit::initialize()
{
    session_.reset(create_session());
}

void SQLTestUnit::finalize() { }


void SQLTestUnit::test_create()
{
    std::cout << '\n';

    session_->open();

    query q(session_->db());

    std::unique_ptr<result> res(q.create<Item>("item").execute());

    q.reset();

    Item hans("Hans", 4711);
    res.reset(q.insert(&hans, "item").execute());

    q.reset();

    res.reset(q.select<Item>().from("item").execute());


    std::for_each(res->begin(), res->end(), [](res))
        while(res->fetch()) {

            q.reset();

    res.reset(q.drop("item").execute());

    }
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
