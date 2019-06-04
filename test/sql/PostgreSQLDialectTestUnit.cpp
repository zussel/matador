//
// Created by sascha on 04.06.19.
//

#include <matador/sql/query.hpp>

#include "../person.hpp"

#include "connections.hpp"

#include "matador/sql/connection.hpp"
#include "matador/sql/sql.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/dialect_token.hpp"

#include "PostgreSQLDialectTestUnit.hpp"

using namespace matador;

PostgreSQLDialectTestUnit::PostgreSQLDialectTestUnit()
  : unit_test("postgresql_dialect" , "postgresql dialect test")
{
  add_test("placeholder", std::bind(&PostgreSQLDialectTestUnit::test_placeholder, this), "test postgresql placeholder link");
  add_test("tablename", std::bind(&PostgreSQLDialectTestUnit::test_table_name, this), "test postgresql extract table name");
}

void PostgreSQLDialectTestUnit::test_placeholder()
{
  matador::connection conn(::connection::postgresql);

  sql s;

  s.append(new detail::insert("person"));

  std::unique_ptr<matador::columns> cols(new columns(columns::WITH_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  std::unique_ptr<matador::detail::values> vals(new detail::values);

  unsigned long id(8);
  std::string name("hans");
  unsigned int age(25);

  vals->push_back(std::make_shared<value<unsigned long>>(id));
  vals->push_back(std::make_shared<value<std::string>>(name));
  vals->push_back(std::make_shared<value<unsigned int>>(age));

  s.append(vals.release());

  std::string result = conn.dialect()->prepare(s);

  UNIT_ASSERT_EQUAL("INSERT INTO \"person\" (\"id\", \"name\", \"age\") VALUES ($1, $2, $3) ", result, "insert statement isn't as expected");
}

void PostgreSQLDialectTestUnit::test_table_name()
{
  query<person> q("person");

  column id("id");

  q.select();

  UNIT_ASSERT_EQUAL("person", q.stmt().table_name(), "table names must be equal");
}
