#include "SessionFixture.hpp"

#include "connection.hpp"

#include "catch2/catch_test_macros.hpp"

namespace matador::test {

SessionFixture::SessionFixture()
: db(connection::dns)
, manager(schema) {
  REQUIRE(db.open());
}

SessionFixture::~SessionFixture() {
  const auto result = manager.drop(db);
  REQUIRE(result.is_ok());
  REQUIRE(db.close());
}

void SessionFixture::drop_table_if_exists(const std::string &table_name) const {
  if (db.exists(table_name)) {
    REQUIRE(query::drop().table(table_name).execute(db));
  }
}

}