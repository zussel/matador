#include "DialectTestUnit.hpp"

#include "TestDialect.hpp"

#include "matador/sql/sql.hpp"
#include "matador/sql/dialect_token.hpp"
#include "matador/sql/columns.hpp"
#include "matador/sql/condition.hpp"

using namespace matador;

DialectTestUnit::DialectTestUnit()
  : unit_test("dialect", "dialect test unit")
{
  add_test("escaping", [this] { test_escaping_quotes(); }, "test dialect escaping");
  add_test("create", [this] { test_create_query(); }, "test create dialect");
  add_test("drop", [this] { test_drop_query(); }, "test drop dialect");
  add_test("insert", [this] { test_insert_query(); }, "test insert dialect");
  add_test("insert_prepare", [this] { test_insert_prepare_query(); }, "test prepared insert dialect");
  add_test("select_all", [this] { test_select_all_query(); }, "test select all dialect");
  add_test("select_distinct", [this] { test_select_distinct_query(); }, "test select distinct dialect");
  add_test("select_limit", [this] { test_select_limit_query(); }, "test select limit dialect");
  add_test("select_ordered", [this] { test_select_ordered_query(); }, "test select ordered dialect");
  add_test("select_grouped", [this] { test_select_grouped_query(); }, "test select grouped dialect");
  add_test("select_where", [this] { test_select_where_query(); }, "test select where dialect");
  add_test("update", [this] { test_update_query(); }, "test update dialect");
  add_test("update_where", [this] { test_update_where_query(); }, "test update where dialect");
  add_test("update_prepare", [this] { test_update_prepare_query(); }, "test prepared update dialect");
  add_test("update_where_prepare", [this] { test_update_where_prepare_query(); }, "test prepared update where dialect");
  add_test("delete", [this] { test_delete_query(); }, "test delete dialect");
  add_test("delete_where", [this] { test_delete_where_query(); }, "test delete where dialect");
}

void DialectTestUnit::test_escaping_quotes()
{
  TestDialect dialect;

  std::string str("baba\"gaga");

  dialect.escape_quotes_in_identifier(str);

  UNIT_ASSERT_EQUAL("baba\"\"gaga", str);
}

void DialectTestUnit::test_create_query()
{
  sql s;

  s.reset(t_query_command::CREATE);
  s.append(std::make_shared<detail::create>("person"));

  auto cols = std::make_shared<columns>(columns::WITH_BRACKETS);

  cols->push_back(make_pk_column("id", data_type::type_unsigned_long, 0));
  cols->push_back(make_column("name", data_type::type_varchar, 1, 255));
  cols->push_back(make_column("age", data_type::type_unsigned_int, 2));

  s.append(cols);

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("create", s.command());
  UNIT_ASSERT_EQUAL("CREATE TABLE \"person\" (\"id\" INTEGER NOT NULL PRIMARY KEY, \"name\" VARCHAR(255), \"age\" INTEGER) ", result);
}

void DialectTestUnit::test_drop_query()
{
  sql s;

  s.reset(t_query_command::DROP);
  s.append(std::make_shared<detail::drop>("person"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("drop", s.command());
  UNIT_ASSERT_EQUAL("DROP TABLE \"person\" ", result);
}

void DialectTestUnit::test_insert_query()
{
  sql s;

  s.reset(t_query_command::INSERT);
  s.table_name("person");
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

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("person", s.table_name());
  UNIT_ASSERT_EQUAL("insert", s.command());
  UNIT_ASSERT_EQUAL("INSERT INTO \"person\" (\"id\", \"name\", \"age\") VALUES (8, 'hans', 25) ", result);
}

void DialectTestUnit::test_insert_prepare_query()
{
  sql s;

  s.reset(t_query_command::INSERT);
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

  TestDialect dialect;
  auto result = dialect.prepare(s);

  UNIT_ASSERT_EQUAL("INSERT INTO \"person\" (\"id\", \"name\", \"age\") VALUES (?, ?, ?) ", result.sql);
}

void DialectTestUnit::test_select_all_query()
{
  sql s;

  s.reset(t_query_command::SELECT);
  s.append(std::make_shared<detail::select>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols);

  s.append(std::make_shared<detail::from>("person"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" ", result);
}

void DialectTestUnit::test_select_distinct_query()
{
  sql s;

  s.reset(t_query_command::SELECT);
  s.append(std::make_shared<detail::select>());
  s.append(std::make_shared<detail::distinct>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols);

  s.append(std::make_shared<detail::from>("person"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT DISTINCT \"id\", \"name\", \"age\" FROM \"person\" ", result);
}

void DialectTestUnit::test_select_limit_query()
{
  sql s;

  s.reset(t_query_command::SELECT);
  s.append(std::make_shared<detail::select>());
  s.append(std::make_shared<detail::top>(10));

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols);

  s.append(std::make_shared<detail::from>("person"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT LIMIT 10 \"id\", \"name\", \"age\" FROM \"person\" ", result);
}

void DialectTestUnit::test_select_ordered_query()
{
  sql s;

  s.reset(t_query_command::SELECT);
  s.append(std::make_shared<detail::select>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols);

  s.append(std::make_shared<detail::from>("person"));
  s.append(std::make_shared<detail::order_by>("name"));
  s.append(std::make_shared<detail::desc>());

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" ORDER BY \"name\" DESC ", result);
}

void DialectTestUnit::test_select_grouped_query()
{
  sql s;

  s.reset(t_query_command::SELECT);
  s.append(std::make_shared<detail::select>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols);

  s.append(std::make_shared<detail::from>("person"));
  s.append(std::make_shared<detail::group_by>("name"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" GROUP BY \"name\" ", result);
}

void DialectTestUnit::test_select_where_query()
{
  sql s;

  s.reset(t_query_command::SELECT);
  s.append(std::make_shared<detail::select>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols);

  s.append(std::make_shared<detail::from>("person"));

  matador::column name("name");
  s.append(std::make_shared<detail::where>(name != "hans"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" WHERE \"name\" <> 'hans' ", result);

  s.reset(t_query_command::SELECT);

  s.append(std::make_shared<detail::select>());

  cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols);

  s.append(std::make_shared<detail::from>("person"));

  s.append(std::make_shared<detail::where>(name != "Hans" && name != "Dieter"));

  result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("select", s.command());
  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" WHERE (\"name\" <> 'Hans' AND \"name\" <> 'Dieter') ", result);
}

void DialectTestUnit::test_update_query()
{
  sql s;

  s.reset(t_query_command::UPDATE);
  s.append(std::make_shared<detail::update>());
  s.append(std::make_shared<detail::tablename>("person"));
  s.append(std::make_shared<detail::set>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  std::string dieter("Dieter");
  unsigned int age54(54);
  cols->push_back(make_column("name", dieter, 255));
  cols->push_back(make_column("age", age54));

  s.append(cols);

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("update", s.command());
  UNIT_ASSERT_EQUAL("UPDATE \"person\" SET \"name\"='Dieter', \"age\"=54 ", result);
}

void DialectTestUnit::test_update_where_query()
{
  sql s;

  s.reset(t_query_command::UPDATE);
  s.append(std::make_shared<detail::update>());
  s.append(std::make_shared<detail::tablename>("person"));
  s.append(std::make_shared<detail::set>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  std::string dieter("Dieter");
  unsigned int age54(54);
  cols->push_back(make_column("name", dieter, 255));
  cols->push_back(make_column("age", age54));

  s.append(cols);

  matador::column name("name");
  matador::column age("age");
  s.append(std::make_shared<detail::where>(name != "Hans" && matador::in(age, {7,5,5,8})));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("update", s.command());
  UNIT_ASSERT_EQUAL("UPDATE \"person\" SET \"name\"='Dieter', \"age\"=54 WHERE (\"name\" <> 'Hans' AND \"age\" IN (7,5,5,8)) ", result);
}

void DialectTestUnit::test_update_prepare_query()
{
  sql s;

  s.reset(t_query_command::UPDATE);
  s.append(std::make_shared<detail::update>());
  s.append(std::make_shared<detail::tablename>("person"));
  s.append(std::make_shared<detail::set>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  std::string dieter("Dieter");
  unsigned int age54(54);
  cols->push_back(make_column("name", dieter, 255));
  cols->push_back(make_column("age", age54));

  s.append(cols);

  TestDialect dialect;
  auto result = dialect.prepare(s);

  UNIT_ASSERT_EQUAL("UPDATE \"person\" SET \"name\"=?, \"age\"=? ", result.sql);
}

void DialectTestUnit::test_update_where_prepare_query()
{
  sql s;

  s.reset(t_query_command::UPDATE);
  s.append(std::make_shared<detail::update>());
  s.append(std::make_shared<detail::tablename>("person"));
  s.append(std::make_shared<detail::set>());

  auto cols = std::make_shared<columns>(columns::WITHOUT_BRACKETS);

  std::string dieter("Dieter");
  unsigned int age54(54);
  cols->push_back(make_column("name", dieter, 255));
  cols->push_back(make_column("age", age54));

  s.append(cols);

  matador::column name("name");
  matador::column age("age");
  s.append(std::make_shared<detail::where>(name != "Hans" && matador::in(age, {7,5,5,8})));

  TestDialect dialect;
  auto result = dialect.prepare(s);

  UNIT_ASSERT_EQUAL("UPDATE \"person\" SET \"name\"=?, \"age\"=? WHERE (\"name\" <> ? AND \"age\" IN (?,?,?,?)) ", result.sql);

}

void DialectTestUnit::test_delete_query()
{
  sql s;

  s.reset(t_query_command::REMOVE);
  s.append(std::make_shared<detail::remove>());
  s.append(std::make_shared<detail::from>("person"));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("delete", s.command());
  UNIT_ASSERT_EQUAL("DELETE FROM \"person\" ", result);
}

void DialectTestUnit::test_delete_where_query()
{
  sql s;

  s.reset(t_query_command::REMOVE);
  s.append(std::make_shared<detail::remove>());
  s.append(std::make_shared<detail::from>("person"));

  matador::column name("name");
  matador::column age("age");
  s.append(std::make_shared<detail::where>(name != "Hans" && matador::between(age, 21, 30)));

  TestDialect dialect;
  std::string result = dialect.direct(s);

  UNIT_ASSERT_EQUAL("DELETE FROM \"person\" WHERE (\"name\" <> 'Hans' AND \"age\" BETWEEN 21 AND 30) ", result);
}
