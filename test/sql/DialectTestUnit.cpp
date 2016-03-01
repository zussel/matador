//
// Created by sascha on 2/26/16.
//

#include "DialectTestUnit.hpp"
#include "TestDialect.hpp"

#include "sql/sql.hpp"

using namespace oos;

DialectTestUnit::DialectTestUnit()
  : unit_test("dialect", "dialect test unit")
{
  add_test("create", std::bind(&DialectTestUnit::test_create_query, this), "test create a dialect");
  add_test("drop", std::bind(&DialectTestUnit::test_drop_query, this), "test drop a dialect");
  add_test("insert", std::bind(&DialectTestUnit::test_insert_query, this), "test insert a dialect");
}

void DialectTestUnit::test_create_query()
{
  sql s;

  s.append(new detail::create("person"));

  std::unique_ptr<oos::detail::columns> cols(new detail::columns);

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

  std::unique_ptr<oos::detail::columns> cols(new detail::columns);

  cols->push_back(std::make_shared<detail::column>("id"));
  cols->push_back(std::make_shared<detail::column>("name"));
  cols->push_back(std::make_shared<detail::column>("age"));

  s.append(cols.release());

  std::unique_ptr<oos::detail::values> vals(new detail::values);

  vals->push_back(std::make_shared(detail::value<unsigned long>>))

}
