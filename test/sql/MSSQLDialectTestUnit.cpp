//
// Created by sascha on 6/13/16.
//

#include "MSSQLDialectTestUnit.hpp"

#include "connections.hpp"
#include "../Item.hpp"

#include "sql/sql.hpp"
#include "sql/query.hpp"
#include "sql/dialect_token.hpp"
#include "sql/connection.hpp"

using namespace oos;

MSSQLDialectTestUnit::MSSQLDialectTestUnit()
  : unit_test("mssql_dialect", "mssql dialect text")
{
  add_test("limit", std::bind(&MSSQLDialectTestUnit::test_limit, this), "test mssql limit compile");
  add_test("sub_select", std::bind(&MSSQLDialectTestUnit::test_query_select_sub_select, this), "test query sub select");
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

  UNIT_ASSERT_EQUAL("SELECT TOP (10) id, name, age FROM person ", result, "select limit isn't as expected");
}

void MSSQLDialectTestUnit::test_query_select_sub_select()
{
  oos::connection conn(connection::mssql);

  conn.open();

  query<person> q("person");

  // create item table and insert item
  result<person> res(q.create().execute(conn));

  unsigned long counter = 0;

  std::unique_ptr<person> hans(new person(++counter, "Hans", oos::date(12, 3, 1980), 180));
  res = q.insert(hans.get()).execute(conn);

  std::unique_ptr<person> otto(new person(++counter, "Otto", oos::date(27, 11, 1954), 159));
  res = q.insert(otto.get()).execute(conn);

  std::unique_ptr<person> hilde(new person(++counter, "Hilde", oos::date(13, 4, 1975), 175));
  res = q.insert(hilde.get()).execute(conn);

  std::unique_ptr<person> trude(new person(++counter, "Trude", oos::date(1, 9, 1967), 166));
  res = q.insert(trude.get()).execute(conn);

  column id("id");

  auto subselect = oos::select(columns::all()).from(oos::select({id}).from("person").limit(1)).as("p");
//  std::cout << "\nSQL: " << subselect.str(*conn, false) << "\n";
//  std::cout << "SQL: " << q.select().where(oos::in(id, subselect)).str(*conn, false) << "\n";

  res = q.select().where(oos::in(id, subselect, conn.dialect())).execute(conn);

  auto first = res.begin();
  auto last = res.end();

  while (first != last) {
    std::unique_ptr<person> item(first.release());
    UNIT_EXPECT_EQUAL(1UL, item->id(), "invalid value");
    UNIT_EXPECT_EQUAL("Hans", item->name(), "invalid value");
    ++first;
  }

  q.drop().execute(conn);

  conn.close();
}
