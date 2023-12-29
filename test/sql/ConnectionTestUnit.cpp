#include "ConnectionTestUnit.hpp"

#include "matador/sql/connection.hpp"
#include "matador/sql/connection_info.hpp"
#include "matador/sql/database_error.hpp"

using namespace matador;
using namespace std;

ConnectionTestUnit::ConnectionTestUnit(const std::string &prefix, std::string dns)
  : unit_test(prefix + "_conn", prefix + " connection test unit")
  , dns_(std::move(dns))
  , db_vendor_(prefix)
{
  add_test("open_close", [this] { test_open_close(); }, "connect sql test");
  add_test("reopen", [this] { test_reopen(); }, "reopen sql test");
  add_test("client_version", [this] { test_client_version(); }, "client version test");
  add_test("server_version", [this] { test_server_version(); }, "server version test");
  add_test("reconnect", [this] { test_reconnect(); }, "reconnect sql test");
  add_test("connection_failed", [this] { test_connection_failed(); }, "connection failed test");
}

void ConnectionTestUnit::test_open_close()
{
  matador::connection conn(dns_);

  UNIT_ASSERT_FALSE(conn.is_connected());

  conn.connect();

  UNIT_ASSERT_TRUE(conn.is_connected());

  conn.disconnect();

  UNIT_ASSERT_FALSE(conn.is_connected());
}

void ConnectionTestUnit::test_client_version()
{
  matador::connection conn(dns_);

  UNIT_ASSERT_FALSE(conn.is_connected());

  const auto v = conn.client_version();

  UNIT_ASSERT_GREATER(v, {});
}

void ConnectionTestUnit::test_server_version()
{
  matador::connection conn(dns_);

  UNIT_ASSERT_FALSE(conn.is_connected());

  if (db_vendor_ != "sqlite") {
    UNIT_ASSERT_EXCEPTION(conn.server_version(), matador::database_error, "not connected");
  }

  conn.connect();

  UNIT_ASSERT_TRUE(conn.is_connected());

  const auto v = conn.server_version();

  UNIT_ASSERT_GREATER(v, {});

  conn.disconnect();

  UNIT_ASSERT_FALSE(conn.is_connected());
}

void ConnectionTestUnit::test_reopen()
{
  matador::connection conn(dns_);

  UNIT_ASSERT_FALSE(conn.is_connected());

  conn.connect();

  UNIT_ASSERT_TRUE(conn.is_connected());

  conn.disconnect();

  conn.connect();

  UNIT_ASSERT_TRUE(conn.is_connected());

  conn.disconnect();

  UNIT_ASSERT_FALSE(conn.is_connected());
}

void ConnectionTestUnit::test_reconnect()
{
  matador::connection conn(dns_);

  UNIT_ASSERT_FALSE(conn.is_connected());

  conn.connect();

  UNIT_ASSERT_TRUE(conn.is_connected());

  conn.reconnect();

  UNIT_ASSERT_TRUE(conn.is_connected());

  conn.disconnect();

  UNIT_ASSERT_FALSE(conn.is_connected());
}

void ConnectionTestUnit::test_connection_failed()
{
  if (db_vendor_ == "sqlite") {
    UNIT_INFO("skipping connection fail test for sqlite");
    return;
  }

  auto ci = matador::connection_info::parse(dns_, 0);
  ci.database = "invalid";
  const auto dns = matador::connection_info::to_string(ci);

  matador::connection conn(dns);

  bool caught_exception = false;
  try {
    conn.connect();
  } catch (database_error &ex) {
    caught_exception = true;
    UNIT_INFO(dns);
    UNIT_INFO(ex.what());
    UNIT_INFO(ex.sql_state());
    UNIT_EXPECT_EQUAL("42000", ex.sql_state());
  } catch (...) {
    UNIT_FAIL("caught from exception");
  }
  UNIT_ASSERT_TRUE(caught_exception);
}
