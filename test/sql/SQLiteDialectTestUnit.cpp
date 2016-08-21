//
// Created by sascha on 29.07.16.
//

#include "SQLiteDialectTestUnit.hpp"

#include "connections.hpp"

#include "sql/connection.hpp"
#include "sql/column.hpp"
#include "sql/dialect_token.hpp"

using namespace oos;

SQLiteDialectTestUnit::SQLiteDialectTestUnit()
  : unit_test("sqlite_dialect", "sqlite dialect text")
{
  add_test("update_limit", std::bind(&SQLiteDialectTestUnit::test_update_with_limit, this), "test sqlite update limit compile");
}

void SQLiteDialectTestUnit::test_update_with_limit()
{
  oos::connection conn(connection::sqlite);

  sql s;

  s.append(new detail::update);
  s.append(new detail::tablename("person"));
  s.append(new detail::set);

  std::unique_ptr<oos::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  std::string dieter("Dieter");
  unsigned int age54(54);
  cols->push_back(std::make_shared<detail::value_column<std::string>>("name", dieter));
  cols->push_back(std::make_shared<detail::value_column<unsigned int>>("age", age54));

  s.append(cols.release());

  oos::column name("name");
  s.append(new detail::where(name != "Hans"));

  s.append(new detail::top(1));

  std::string result = conn.dialect()->direct(s);

  UNIT_ASSERT_EQUAL("UPDATE person SET name='Dieter', age=54 WHERE rowid IN (SELECT rowid FROM person WHERE name <> 'Hans' LIMIT 1 ) ", result, "update where isn't as expected");
}
