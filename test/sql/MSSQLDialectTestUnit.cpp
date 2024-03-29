#include "MSSQLDialectTestUnit.hpp"

#include "connections.hpp"

#include "../person.hpp"

#include "matador/sql/sql.hpp"
#include "matador/sql/query.hpp"
#include "matador/sql/dialect_token.hpp"
#include "matador/sql/connection.hpp"

using namespace matador;

MSSQLDialectTestUnit::MSSQLDialectTestUnit()
  : unit_test("mssql_dialect", "mssql dialect test")
{
  add_test("limit", std::bind(&MSSQLDialectTestUnit::test_limit, this), "test mssql limit compile");
  add_test("sub_select", std::bind(&MSSQLDialectTestUnit::test_query_select_sub_select, this), "test query sub select");
  //add_test("sub_select_result", std::bind(&MSSQLDialectTestUnit::test_query_select_sub_select_result, this), "test query sub select result");
}

void MSSQLDialectTestUnit::test_limit()
{
  matador::connection conn(::connection::mssql);

  sql s;

  s.append(std::make_shared<detail::select>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols);

  s.append(std::make_shared<detail::from>("person"));

  s.append(std::make_shared<detail::top>(10));

  std::string result = conn.dialect()->direct(s);

  UNIT_ASSERT_EQUAL("SELECT TOP (10) [id], [name], [age] FROM [person] ", result);
}

void MSSQLDialectTestUnit::test_query_select_sub_select()
{
  matador::connection conn(::connection::mssql);

  query<person> q;

  column id("id");

  auto subselect = matador::select(columns::all()).from(matador::select({id}).from("person").limit(1)).as("p");

  q.select().from("person").where(matador::in(id, subselect));

  UNIT_ASSERT_EQUAL("SELECT [id], [name], [birthdate], [height] FROM [person] WHERE [id] IN (SELECT * FROM (SELECT TOP (1) [id] FROM [person] ) AS p ) ", q.str(conn, false));
}

void MSSQLDialectTestUnit::test_query_select_sub_select_result()
{
  matador::connection conn(::connection::mssql);

  conn.connect();

  query<person> q;

  // create item table and insert item
  result<person> res(q.create("person").execute(conn));

  unsigned long counter = 0;

  person hans(++counter, "Hans", matador::date(12, 3, 1980), 180);
  res = q.insert("person", hans).execute(conn);

  person otto(++counter, "Otto", matador::date(27, 11, 1954), 159);
  res = q.insert("person", otto).execute(conn);

  person hilde(++counter, "Hilde", matador::date(13, 4, 1975), 175);
  res = q.insert("person", hilde).execute(conn);

  person trude(++counter, "Trude", matador::date(1, 9, 1967), 166);
  res = q.insert("person", trude).execute(conn);

  column id("id");

  auto subselect = matador::select(columns::all()).from(matador::select({id}).from("person").limit(1)).as("p");

  res = q.select().from("person").where(matador::in(id, subselect)).execute(conn);

  //for (auto p : res) {
  //    UNIT_EXPECT_EQUAL(1UL, p->id());
  //    UNIT_EXPECT_EQUAL("Hans", p->name());
  //}
  q.drop("person").execute(conn);

  conn.disconnect();
}
