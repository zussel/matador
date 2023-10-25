#include "SQLiteDialectTestUnit.hpp"

#include "connections.hpp"

#include "matador/sql/connection.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/columns.hpp"
#include "matador/sql/dialect_token.hpp"
#include "matador/sql/sql.hpp"

using namespace matador;

SQLiteDialectTestUnit::SQLiteDialectTestUnit()
  : unit_test("sqlite_dialect", "sqlite dialect test")
{
  add_test("update_limit", [this] { test_update_with_limit(); }, "test sqlite update limit compile");
  add_test("delete_limit", [this] { test_delete_with_limit(); }, "test sqlite delete limit compile");
}

void SQLiteDialectTestUnit::test_update_with_limit()
{
  matador::connection conn(::connection::sqlite);

  sql s;

  s.append(std::make_unique<detail::update>());
  s.append(std::make_unique<detail::tablename>("person"));
  s.append(std::make_unique<detail::set>());

  auto cols = std::make_unique<columns>(columns::WITHOUT_BRACKETS);

  std::string dieter("Dieter");
  unsigned int age54(54);
  cols->push_back(make_column("name", dieter, 255));
  cols->push_back(make_column("age", age54));

  s.append(std::move(cols));

  matador::column name("name");
  s.append(std::make_unique<detail::where>(name != "Hans"));

  s.append(std::make_unique<detail::top>(1));

  std::string result = conn.dialect()->direct(s);

  UNIT_ASSERT_EQUAL("UPDATE \"person\" SET \"name\"='Dieter', \"age\"=54 WHERE \"rowid\" IN (SELECT \"rowid\" FROM \"person\" WHERE \"name\" <> 'Hans' LIMIT 1 ) ", result);
}

void SQLiteDialectTestUnit::test_delete_with_limit()
{
  matador::connection conn(::connection::sqlite);

  sql s;

  s.append(std::make_unique<detail::remove>());
  s.append(std::make_unique<detail::from>("person"));

  matador::column name("name");
  s.append(std::make_unique<detail::where>(name != "Hans"));

  s.append(std::make_unique<detail::top>(1));

  std::string result = conn.dialect()->direct(s);

  UNIT_ASSERT_EQUAL("DELETE FROM \"person\" WHERE \"rowid\" IN (SELECT \"rowid\" FROM \"person\" WHERE \"name\" <> 'Hans' LIMIT 1 ) ", result);
}
