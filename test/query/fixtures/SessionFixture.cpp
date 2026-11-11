#include "SessionFixture.hpp"

#include "../backend/test_backend_service.hpp"

#include "matador/query/interface/connection_impl.hpp"
#include "matador/query/runtime/connection.hpp"

#include "catch2/catch_test_macros.hpp"

namespace matador::test {
SessionFixture::SessionFixture() {
  query::backend_provider::instance().register_backend("noop", std::make_unique<orm::test_backend_service>());

  db = std::make_unique<query::connection>("noop://noop.db");

  REQUIRE(db->open());
}

SessionFixture::~SessionFixture() {
  REQUIRE(db->close());
}

query::session_context SessionFixture::make_session_context(const size_t connection_count) {
  return query::session_context(bus, "noop://noop.db", connection_count);
}
} // namespace matador::test