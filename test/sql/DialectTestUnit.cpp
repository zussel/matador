//
// Created by sascha on 2/26/16.
//

#include "DialectTestUnit.hpp"

#include "TestDialect.hpp"

#include "sql/sql.hpp"
#include "sql/condition.hpp"

using namespace oos;

DialectTestUnit::DialectTestUnit()
  : unit_test("dialect", "dialect test unit")
{
  add_test("create", std::bind(&DialectTestUnit::test_create_query, this), "test create dialect");
  add_test("drop", std::bind(&DialectTestUnit::test_drop_query, this), "test drop dialect");
  add_test("insert", std::bind(&DialectTestUnit::test_insert_query, this), "test insert dialect");
  add_test("select_all", std::bind(&DialectTestUnit::test_select_all_query, this), "test select all dialect");
  add_test("select_ordered", std::bind(&DialectTestUnit::test_select_ordered_query, this), "test select ordered dialect");
  add_test("select_grouped", std::bind(&DialectTestUnit::test_select_grouped_query, this), "test select grouped dialect");
  add_test("select_where", std::bind(&DialectTestUnit::test_select_where_query, this), "test select where dialect");
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

  cols->push_back(std::make_shared<detail::column>("id"));
  cols->push_back(std::make_shared<detail::column>("name"));
  cols->push_back(std::make_shared<detail::column>("age"));

  s.append(cols.release());

  std::unique_ptr<oos::detail::values> vals(new detail::values);

  vals->push_back(std::make_shared<detail::value<unsigned long>>(8));
  vals->push_back(std::make_shared<detail::value<std::string>>("hans"));
  vals->push_back(std::make_shared<detail::value<unsigned int>>(25));

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

  cols->push_back(std::make_shared<detail::column>("id"));
  cols->push_back(std::make_shared<detail::column>("name"));
  cols->push_back(std::make_shared<detail::column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("SELECT id, name, age FROM person ", result, "select isn't as expected");
}

void DialectTestUnit::test_select_ordered_query()
{
  sql s;

  s.append(new detail::select);

  std::unique_ptr<oos::detail::columns> cols(new detail::columns(detail::columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<detail::column>("id"));
  cols->push_back(std::make_shared<detail::column>("name"));
  cols->push_back(std::make_shared<detail::column>("age"));

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

  cols->push_back(std::make_shared<detail::column>("id"));
  cols->push_back(std::make_shared<detail::column>("name"));
  cols->push_back(std::make_shared<detail::column>("age"));

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

  cols->push_back(std::make_shared<detail::column>("id"));
  cols->push_back(std::make_shared<detail::column>("name"));
  cols->push_back(std::make_shared<detail::column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  oos::field name("name");
  auto c = name != "hans";

  s.append(new detail::where(c));

  TestDialect dialect;
  std::string result = s.compile(dialect, detail::token::DIRECT);

  UNIT_ASSERT_EQUAL("SELECT id, name, age FROM person WHERE ", result, "select isn't as expected");
}
