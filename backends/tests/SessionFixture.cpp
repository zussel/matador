#include "SessionFixture.hpp"

namespace matador::test {

SessionFixture::SessionFixture()
  : pool(connection::dns, 4), ses(pool) {}

SessionFixture::~SessionFixture() {
  while (!tables_to_drop.empty()) {
    drop_table_if_exists(tables_to_drop.top());
    tables_to_drop.pop();
  }
}

void SessionFixture::drop_table_if_exists(const std::string &table_name) const {
  if (ses.table_exists(table_name)) {
    ses.drop_table(table_name);
  }
}

}