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
  add_test("placeholder", [this] { test_placeholder(); }, "test postgresql placeholder link");
  add_test("placeholder_condition", [this] { test_placeholder_condition(); }, "test postgresql placeholder in condition link");
  add_test("update_limit", [this] { test_update_limit(); }, "test postgresql limit");
  add_test("update_limit_prepare", [this] { test_update_limit_prepare(); }, "test postgresql prepared limit");
  add_test("tablename", [this] { test_table_name(); }, "test postgresql extract table name");
}

void PostgreSQLDialectTestUnit::test_placeholder()
{
  matador::connection conn(::connection::postgresql);

  sql s;

  s.append(std::make_shared<detail::insert>("person"));

  auto cols = std::make_shared<columns>(columns::WITH_BRACKETS);

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols);

  auto vals = std::make_shared<detail::values>();

  unsigned long id(8);
  std::string name("hans");
  unsigned int age(25);

  vals->push_back(std::make_shared<value>(id));
  vals->push_back(std::make_shared<value>(name));
  vals->push_back(std::make_shared<value>(age));

  s.append(vals);

  auto result = conn.dialect()->prepare(s);

  UNIT_ASSERT_EQUAL("INSERT INTO \"person\" (\"id\", \"name\", \"age\") VALUES ($1, $2, $3) ", result.sql);
}

void PostgreSQLDialectTestUnit::test_placeholder_condition()
{
  matador::connection conn(::connection::postgresql);

  sql s;

  s.append(std::make_shared<detail::select>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols);

  s.append(std::make_shared<detail::from>("person"));

  s.append(std::make_shared<detail::where>("name"_col == "hans"));

  auto result = conn.dialect()->prepare(s);

  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" WHERE \"name\" = $1 ", result.sql);
}

void PostgreSQLDialectTestUnit::test_update_limit()
{
  matador::connection conn(::connection::postgresql);

  sql s;

  s.append(std::make_shared<detail::update>());
  s.append(std::make_shared<detail::tablename>("relation"));
  s.append(std::make_shared<detail::set>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  unsigned long owner_id(1);
  cols->push_back(make_column("owner_id", owner_id, -1));

  s.append(cols);

  s.append(std::make_shared<detail::where>("owner_id"_col == 1 && "item_id"_col == 1));
  s.append(std::make_shared<detail::top>(1));

  auto result = conn.dialect()->direct(s);

  UNIT_ASSERT_EQUAL("UPDATE \"relation\" SET \"owner_id\"=1 WHERE \"ctid\" = (SELECT \"ctid\" FROM \"relation\" WHERE (\"owner_id\" = 1 AND \"item_id\" = 1) LIMIT 1 ) ", result);
}

void PostgreSQLDialectTestUnit::test_update_limit_prepare(){
  matador::connection conn(::connection::postgresql);

  sql s;

  s.append(std::make_shared<detail::update>());
  s.append(std::make_shared<detail::tablename>("relation"));
  s.append(std::make_shared<detail::set>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  unsigned long owner_id(1);
  cols->push_back(make_column("owner_id", owner_id, -1));

  s.append(cols);

  s.append(std::make_shared<detail::where>("owner_id"_col == 1 && "item_id"_col == 1));
  s.append(std::make_shared<detail::top>(1));

  auto result = conn.dialect()->prepare(s);

  UNIT_ASSERT_EQUAL("UPDATE \"relation\" SET \"owner_id\"=$1 WHERE \"ctid\" = (SELECT \"ctid\" FROM \"relation\" WHERE (\"owner_id\" = $2 AND \"item_id\" = $3) LIMIT 1 ) ", result.sql);
}

void PostgreSQLDialectTestUnit::test_table_name()
{
  query<person> q;

  column id("id");

  q.select().from("person");

  UNIT_ASSERT_EQUAL("person", q.stmt().table_name());
}
