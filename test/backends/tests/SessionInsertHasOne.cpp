#include "catch2/catch_test_macros.hpp"

#include "fixtures/SessionFixture.hpp"

#include "connection.hpp"

#include "matador/query/runtime/session.hpp"

#include "models/user.hpp"
#include "models/country.hpp"

using namespace matador;
using namespace matador::test;
using namespace matador::query;

TEST_CASE_METHOD(SessionFixture, "Test insert object with has_one relation lazy", "[session][insert][has_one][lazy]") {
  const auto result = schema.attach<user_identity>("users")
    .and_then( [this] { return schema.attach<user_session_identity>("user_sessions"); } )
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);

  const auto u = make_object<user_identity>("user1", "password");
  auto us = make_object<user_session_identity>("session1", u);

  REQUIRE(u.is_transient());
  REQUIRE(us.is_transient());
  REQUIRE(ses.insert(us).is_ok());
  REQUIRE(us.is_persistent());
  REQUIRE(u.is_persistent());

  const auto user_result = ses.find<user_identity>(u->id);
  REQUIRE(user_result.is_ok());
  us = user_result.value()->session;
  REQUIRE(us->session_token == "session1");
}

TEST_CASE_METHOD(SessionFixture, "Test insert object with has_one relation eager", "[session][insert][has_one][eager]") {
  const auto result = schema.attach<country_identity>("countries")
    .and_then( [this] { return schema.attach<capital_identity>("capitals"); } )
    .and_then([this] { return manager.create(db); } );
  REQUIRE(result.is_ok());

  session ses({bus, test::connection::dns, 4}, schema);

  const auto ger = make_object<country_identity>("Germany");
  auto ger_cap = make_object<capital_identity>("Berlin", ger);

  REQUIRE(ger.is_transient());
  REQUIRE(ger_cap.is_transient());
  REQUIRE(ses.insert(ger_cap).is_ok());
  REQUIRE(ger_cap.is_persistent());
  REQUIRE(ger.is_persistent());

  const auto ger_result = ses.find<country_identity>(ger->id);
  REQUIRE(ger_result.is_ok());
  ger_cap = ger_result.value()->capital;
  REQUIRE(ger_cap->name == "Berlin");
}