#include "QueryFixture.hpp"

namespace matador::test {

QueryFixture::QueryFixture()
  : db(matador::test::connection::dns)
  , schema(db.dialect().default_schema_name())
{
  db.open();
}

QueryFixture::~QueryFixture() {
  for (const auto &table_name: tables_to_drop) {
    drop_table_if_exists(table_name);
  }
  tables_to_drop.clear();
}

void QueryFixture::drop_table_if_exists(const std::string &table_name) {
  if (db.exists(table_name)) {
    db.query(schema).drop().table(table_name).execute();
  }
}

}