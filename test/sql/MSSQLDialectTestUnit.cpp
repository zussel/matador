//
// Created by sascha on 6/13/16.
//

#include "MSSQLDialectTestUnit.hpp"

#include "connections.hpp"

#include "sql/sql.hpp"
#include "sql/dialect_token.hpp"
#include "sql/connection.hpp"

using namespace oos;

MSSQLDialectTestUnit::MSSQLDialectTestUnit()
  : unit_test("mssql_dialect", "mssql dialect text")
{
  add_test("limit", std::bind(&MSSQLDialectTestUnit::test_limit, this), "test mssql limit compile");
}

void MSSQLDialectTestUnit::test_limit()
{
  oos::connection conn(connection::mssql);

  sql s;

  s.append(new detail::select);

  std::unique_ptr<oos::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  s.append(new detail::top(10));

  std::string result = conn.dialect()->direct(s);

  UNIT_ASSERT_EQUAL("SELECT TOP 10 id, name, age FROM person ", result, "select limit isn't as expected");
}
