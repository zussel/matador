#include "QueryFixture.hpp"

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
    db.query(schema).drop().table(table_name).execute();
  }
}

}