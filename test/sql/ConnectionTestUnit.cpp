/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ConnectionTestUnit.hpp"

#include "matador/sql/connection.hpp"
#include "matador/sql/database_error.hpp"

#include <fstream>
#include <regex>

using namespace matador;
using namespace std;

ConnectionTestUnit::ConnectionTestUnit(const std::string &prefix, std::string dns)
  : unit_test(prefix + "_conn", prefix + " connection test unit")
  , dns_(std::move(dns))
  , db_vendor_(prefix)
{
  add_test("open_close", [this] { test_open_close(); }, "connect sql test");
  add_test("reopen", [this] { test_reopen(); }, "reopen sql test");
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

  std::regex re("(matador_test)");
  string dns = std::regex_replace(dns_, re, "matador_invalid");

  matador::connection conn(dns);

  bool caught_exception = false;
  try {
    conn.connect();
  } catch (database_error &ex) {
    caught_exception = true;
    UNIT_EXPECT_EQUAL("42000", ex.sql_state());
  } catch (...) {
    UNIT_FAIL("caught from exception");
  }
  UNIT_ASSERT_TRUE(caught_exception);
}
