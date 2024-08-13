#include "SessionFixture.hpp"

namespace matador::test {

SessionFixture::SessionFixture()
  : pool(matador::test::connection::dns, 4), ses(pool) {}

SessionFixture::~SessionFixture() {
  for (const auto &table_name: tables_to_drop) {
    drop_table_if_exists(table_name);
  }
  tables_to_drop.clear();
}

void SessionFixture::drop_table_if_exists(const std::string &table_name) {
  if (ses.table_exists(table_name)) {
    ses.drop_table(table_name);
  }
}

}