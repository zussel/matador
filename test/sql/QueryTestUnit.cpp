//
// Created by sascha on 2/26/16.
//

#include "QueryTestUnit.hpp"
#include "TestDialect.hpp"

#include "sql/sql.hpp"

using namespace oos;

QueryTestUnit::QueryTestUnit()
  : unit_test("query", "query test unit")
{
  add_test("create", std::bind(&QueryTestUnit::test_create_query, this), "test create a query");
}

void QueryTestUnit::test_create_query()
{
  sql s;

  s.append(new detail::create("person"));

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("CREATE TABLE person", result, "create statement isn't as expected");
}



