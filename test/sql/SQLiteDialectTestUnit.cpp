#include "SQLiteDialectTestUnit.hpp"

#include "connections.hpp"

#include "matador/sql/connection.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/dialect_token.hpp"

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

  s.append(new detail::update);
  s.append(new detail::tablename("person"));
  s.append(new detail::set);

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  std::string dieter("Dieter");
  unsigned int age54(54);
  cols->push_back(detail::make_value_column("name", dieter, 255));
  cols->push_back(detail::make_value_column("age", age54, -1));

  s.append(cols.release());

  matador::column name("name");
  s.append(new detail::where(name != "Hans"));

  s.append(new detail::top(1));

  std::string result = conn.dialect()->direct(s);

  UNIT_ASSERT_EQUAL("UPDATE \"person\" SET \"name\"='Dieter', \"age\"=54 WHERE \"rowid\" IN (SELECT \"rowid\" FROM \"person\" WHERE \"name\" <> 'Hans' LIMIT 1 ) ", result);
}

void SQLiteDialectTestUnit::test_delete_with_limit()
{
  matador::connection conn(::connection::sqlite);

  sql s;

  s.append(new detail::remove());
  s.append(new detail::from("person"));

  matador::column name("name");
  s.append(new detail::where(name != "Hans"));

  s.append(new detail::top(1));

  std::string result = conn.dialect()->direct(s);

  UNIT_ASSERT_EQUAL("DELETE FROM \"person\" WHERE \"rowid\" IN (SELECT \"rowid\" FROM \"person\" WHERE \"name\" <> 'Hans' LIMIT 1 ) ", result);
}
