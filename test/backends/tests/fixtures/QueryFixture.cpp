#include "QueryFixture.hpp"

#include "matador/query/builder/query.hpp"

#include "matador/query/dialect/dialect.hpp"

#include "connection.hpp"

#include "catch2/catch_test_macros.hpp"

namespace matador::test {
QueryFixture::QueryFixture()
: db(connection::dns)
, repo(db.dialect().default_schema_name())
, manager(repo) {
  REQUIRE(db.open());
}

QueryFixture::~QueryFixture() {
  while (!tables_to_drop.empty()) {
    drop_table_if_exists(tables_to_drop.top());
    tables_to_drop.pop();
  }
  REQUIRE(manager.drop(db));
  REQUIRE(db.close());
}

void QueryFixture::check_table_exists(const std::string &table_name) const {
  auto result = db.exists(table_name);
  REQUIRE(result.is_ok());
  REQUIRE(*result);
}

void QueryFixture::check_table_not_exists(const std::string &table_name) const {
  auto result = db.exists(table_name);
  REQUIRE(result.is_ok());
  REQUIRE(!*result);
}

void QueryFixture::drop_table_if_exists(const std::string &table_name) const {
  const auto result = db.exists(table_name)
  .and_then([&table_name, this](const bool exists) -> matador::result<bool, error> {
    if (exists) {
      auto res = query::drop()
        .table(table_name)
        .execute(db);
      REQUIRE(res);
      this->check_table_not_exists(table_name);
    }
    return ok(true);
  });
  REQUIRE(result);
}
}
