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

void QueryFixture::drop_table_if_exists(const std::string &table_name) const {
  if (db.exists(table_name)) {
    std::ignore = sql::query::drop()
      .table(table_name)
      .execute(db);
    REQUIRE(!db.exists(table_name));
  }
}

}