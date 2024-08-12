#include <catch2/catch_test_macros.hpp>

#include "matador/sql/column.hpp"
#include "matador/sql/condition.hpp"
#include "matador/sql/connection.hpp"
#include "matador/sql/query_builder.hpp"

#include "models/person.hpp"

#include "connection.hpp"

#include <algorithm>
#include <vector>

class QueryStatementFixture {
public:
  QueryStatementFixture()
    : db(matador::test::connection::dns)
      , schema(db.dialect().default_schema_name()) {
    db.open();
  }

  ~QueryStatementFixture() {
    drop_table_if_exists("person");
  }

protected:
  matador::sql::connection db;
  matador::sql::schema schema;

private:
  void drop_table_if_exists(const std::string &table_name) const {
    if (db.exists(table_name)) {
      db.query(schema).drop().table(table_name).execute();
    }
  }
};

using namespace matador::sql;

TEST_CASE_METHOD(QueryStatementFixture, "Test create statement", "[query][statement][create]") {
  schema.attach<matador::test::person>("person");
  auto stmt = db.query(schema)
    .create()
    .table<matador::test::person>("person")
    .prepare();

  auto res = stmt.execute();
  REQUIRE(res == 0);

  REQUIRE(db.exists("person"));
  const std::vector<std::string> cols = {"id", "name", "age", "image"};
  const auto fields = db.describe("person");

  for (const auto &fld : fields) {
    REQUIRE(std::find(cols.begin(), cols.end(), fld.name()) != cols.end());
  }
}

TEST_CASE_METHOD(QueryStatementFixture, "Test insert statement", "[query][statement][insert]") {
}

TEST_CASE_METHOD(QueryStatementFixture, "Test update statement", "[query][statement][update]") {
}

TEST_CASE_METHOD(QueryStatementFixture, "Test delete statement", "[query][statement][delete]") {
}
