//
// Created by sascha on 9/7/15.
//

#include <tools/time.hpp>
#include "SQLTestUnit.hpp"


SQLTestUnit::SQLTestUnit(const std::string &name, const std::string &msg, const std::string &db, const oos::time &timeval)
  : unit_test(name, msg)
{
    add_test("create", std::bind(&SQLTestUnit::test_create, this), "test direct sql create statement");
}

SQLTestUnit::~SQLTestUnit() {}

void SQLTestUnit::initialize() { }

void SQLTestUnit::finalize() { }


void SQLTestUnit::test_create() {

}

std::string SQLTestUnit::db() const {
    return db_;
}
