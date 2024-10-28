#include "QueryFixture.hpp"

#include "matador/sql/query.hpp"

#include "catch2/catch_test_macros.hpp"

namespace matador::test {

QueryFixture::QueryFixture()
  : db(connection::dns)
  , schema(db.dialect().default_schema_name())
{
  db.open();
}

QueryFixture::~QueryFixture() {
    while (!tables_to_drop.empty()) {
        drop_table_if_exists(tables_to_drop.top());
        tables_to_drop.pop();
    }
}

void QueryFixture::check_table_exists(const std::string &table_name) const
{
  auto result = db.exists(table_name);
  REQUIRE(result.is_ok());
  REQUIRE(*result);
}

void QueryFixture::check_table_not_exists(const std::string &table_name) const
{
  auto result = db.exists(table_name);
  REQUIRE(result.is_ok());
  REQUIRE(!*result);
}

void QueryFixture::drop_table_if_exists(const std::string &table_name) const {
  const auto result = db.exists(table_name).and_then([&table_name, this](bool exists) {
    if (exists) {
      if (sql::query::drop()
      .table(table_name)
      .execute(db).is_ok()) {
        this->check_table_not_exists(table_name);
      } else {
        FAIL("Failed to drop table");
      }
    }
    return utils::ok(true);
  });
}

}