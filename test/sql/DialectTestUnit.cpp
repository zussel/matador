//
// Created by sascha on 2/26/16.
//

#include "DialectTestUnit.hpp"

#include "TestDialect.hpp"

#include "sql/sql.hpp"
#include "sql/dialect_token.hpp"
#include "sql/column.hpp"
#include "sql/condition.hpp"

using namespace oos;

DialectTestUnit::DialectTestUnit()
  : unit_test("dialect", "dialect test unit")
{
  add_test("create", std::bind(&DialectTestUnit::test_create_query, this), "test create dialect");
  add_test("drop", std::bind(&DialectTestUnit::test_drop_query, this), "test drop dialect");
  add_test("insert", std::bind(&DialectTestUnit::test_insert_query, this), "test insert dialect");
  add_test("select_all", std::bind(&DialectTestUnit::test_select_all_query, this), "test select all dialect");
  add_test("select_distinct", std::bind(&DialectTestUnit::test_select_distinct_query, this), "test select distinct dialect");
  add_test("select_limit", std::bind(&DialectTestUnit::test_select_limit_query, this), "test select limit dialect");
  add_test("select_ordered", std::bind(&DialectTestUnit::test_select_ordered_query, this), "test select ordered dialect");
  add_test("select_grouped", std::bind(&DialectTestUnit::test_select_grouped_query, this), "test select grouped dialect");
  add_test("select_where", std::bind(&DialectTestUnit::test_select_where_query, this), "test select where dialect");
  add_test("update", std::bind(&DialectTestUnit::test_update_query, this), "test update dialect");
  add_test("update_where", std::bind(&DialectTestUnit::test_update_where_query, this), "test update where dialect");
  add_test("delete", std::bind(&DialectTestUnit::test_delete_query, this), "test delete dialect");
  add_test("delete_where", std::bind(&DialectTestUnit::test_delete_where_query, this), "test delete where dialect");
}

void DialectTestUnit::test_create_query()
{
  sql s;

  s.append(new detail::create("person"));

  std::unique_ptr<oos::detail::columns> cols(new detail::columns(detail::columns::WITH_BRACKETS));

  cols->push_back(std::make_shared<detail::identifier_column>("id", data_type_t::type_unsigned_long, 0, false));
  cols->push_back(std::make_shared<detail::varchar_column>("name", 256, data_type_t::type_varchar, 1, false));
  cols->push_back(std::make_shared<detail::typed_column>("age", data_type_t::type_unsigned_int, 2, false));

  s.append(cols.release());

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("CREATE TABLE person (id INTEGER NOT NULL PRIMARY KEY, name VARCHAR(256), age INTEGER) ", result, "create statement isn't as expected");
}

void DialectTestUnit::test_drop_query()
{
  sql s;

  s.append(new detail::drop("person"));

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("DROP TABLE person ", result, "drop statement isn't as expected");
}

void DialectTestUnit::test_insert_query()
{
  sql s;

  s.append(new detail::insert("person"));

  std::unique_ptr<oos::detail::columns> cols(new detail::columns(detail::columns::WITH_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  std::unique_ptr<oos::detail::values> vals(new detail::values);

  vals->push_back(std::make_shared<value<unsigned long>>(8));
  vals->push_back(std::make_shared<value<std::string>>("hans"));
  vals->push_back(std::make_shared<value<unsigned int>>(25));

  s.append(vals.release());

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("INSERT INTO person (id, name, age) VALUES (8, 'hans', 25) ", result, "insert statement isn't as expected");
}

void DialectTestUnit::test_select_all_query()
{
  sql s;

  s.append(new detail::select);

  std::unique_ptr<oos::detail::columns> cols(new detail::columns(detail::columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("SELECT id, name, age FROM person ", result, "select isn't as expected");
}

void DialectTestUnit::test_select_distinct_query()
{
  sql s;

  s.append(new detail::select);
  s.append(new detail::distinct);

  std::unique_ptr<oos::detail::columns> cols(new detail::columns(detail::columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("SELECT DISTINCT id, name, age FROM person ", result, "select distinct isn't as expected");
}

void DialectTestUnit::test_select_limit_query()
{
  sql s;

  s.append(new detail::select);
  s.append(new detail::top(10));

  std::unique_ptr<oos::detail::columns> cols(new detail::columns(detail::columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("SELECT TOP 10 id, name, age FROM person ", result, "select limit isn't as expected");
}

void DialectTestUnit::test_select_ordered_query()
{
  sql s;

  s.append(new detail::select);

  std::unique_ptr<oos::detail::columns> cols(new detail::columns(detail::columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));
  s.append(new detail::order_by("name"));
  s.append(new detail::desc);

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("SELECT id, name, age FROM person ORDER BY name DESC ", result, "select isn't as expected");
}

void DialectTestUnit::test_select_grouped_query()
{
  sql s;

  s.append(new detail::select);

  std::unique_ptr<oos::detail::columns> cols(new detail::columns(detail::columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));
  s.append(new detail::group_by("name"));

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("SELECT id, name, age FROM person GROUP BY name ", result, "select isn't as expected");
}

void DialectTestUnit::test_select_where_query()
{
  sql s;

  s.append(new detail::select);

  std::unique_ptr<oos::detail::columns> cols(new detail::columns(detail::columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  oos::column name("name");
  s.append(new detail::where(name != "hans"));

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("SELECT id, name, age FROM person WHERE name <> 'hans' ", result, "select isn't as expected");

  s.reset();

  s.append(new detail::select);

  cols.reset(new detail::columns(detail::columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  s.append(new detail::where(name != "Hans" && name != "Dieter"));

  result = s.compile(dialect, detail::token::DIRECT);
  UNIT_ASSERT_EQUAL("SELECT id, name, age FROM person WHERE (name <> 'Hans' AND name <> 'Dieter') ", result, "select isn't as expected");
}

void DialectTestUnit::test_update_query()
{
  sql s;

  s.append(new detail::update("person"));
  s.append(new detail::set);

  std::unique_ptr<oos::detail::columns> cols(new detail::columns(detail::columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<detail::value_column<std::string>>("name", "Dieter"));
  cols->push_back(std::make_shared<detail::value_column<unsigned int>>("age", 54));

  s.append(cols.release());

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("UPDATE person SET name='Dieter', age=54 ", result, "update isn't as expected");
}

void DialectTestUnit::test_update_where_query()
{
  sql s;

  s.append(new detail::update("person"));
  s.append(new detail::set);

  std::unique_ptr<oos::detail::columns> cols(new detail::columns(detail::columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<detail::value_column<std::string>>("name", "Dieter"));
  cols->push_back(std::make_shared<detail::value_column<unsigned int>>("age", 54));

  s.append(cols.release());

  oos::column name("name");
  oos::column age("age");
  s.append(new detail::where(name != "Hans" && oos::in(age, {7,5,5,8})));

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("UPDATE person SET name='Dieter', age=54 WHERE (name <> 'Hans' AND age IN (7,5,5,8)) ", result, "update where isn't as expected");
}

void DialectTestUnit::test_delete_query()
{
  sql s;

  s.append(new detail::remove("person"));

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("DELETE FROM person ", result, "delete isn't as expected");
}

void DialectTestUnit::test_delete_where_query()
{
  sql s;

  s.append(new detail::remove("person"));

  oos::column name("name");
  oos::column age("age");
  s.append(new detail::where(name != "Hans" && oos::between(age, 21, 30)));

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("DELETE FROM person WHERE (name <> 'Hans' AND age BETWEEN 21 AND 30) ", result, "delete where isn't as expected");
}
