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
  add_test("placeholder_condition", std::bind(&PostgreSQLDialectTestUnit::test_placeholder_condition, this), "test postgresql placeholder in condition link");
  add_test("update_limit", std::bind(&PostgreSQLDialectTestUnit::test_update_limit, this), "test postgresql limit");
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

void PostgreSQLDialectTestUnit::test_placeholder_condition()
{
  matador::connection conn(::connection::postgresql);

  sql s;

  s.append(new detail::select());

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  s.append(new detail::where("name"_col == "hans"));

  std::string result = conn.dialect()->prepare(s);

  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" WHERE \"name\" = $1 ", result, "select statement isn't as expected");
}

void PostgreSQLDialectTestUnit::test_update_limit()
{
  std::shared_ptr<columns> update_columns;
  std::vector<matador::any> rowvalues;
  detail::query_value_column_processor query_value_column_processor(update_columns, rowvalues);

  matador::connection conn(::connection::postgresql);

  sql s;

  s.append(new detail::update);
  s.append(new detail::tablename("relation"));
  s.append(new detail::set);

  auto colvalues = std::list<std::pair<std::string, matador::any>>{{"owner_id", 1}};

  for(auto colvalue : colvalues) {
    query_value_column_processor.execute(colvalue);
  }

  s.append(new detail::where("owner_id"_col == 1 && "item_id"_col == 1));
  s.append(new detail::top(1));

  std::string result = conn.dialect()->direct(s);

  UNIT_ASSERT_EQUAL("SELECT \"owner_id\", \"item_id\" FROM \"relation\" WHERE (\"owner_id\" = 1 AND \"item_id\" = 1) LIMIT 1 ", result, "select statement isn't as expected");
}

void PostgreSQLDialectTestUnit::test_table_name()
{
  query<person> q("person");

  column id("id");

  q.select();

  UNIT_ASSERT_EQUAL("person", q.stmt().table_name(), "table names must be equal");
}
