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

#include <fstream>

using namespace matador;
using namespace std;

ConnectionTestUnit::ConnectionTestUnit(const std::string &name, const std::string &msg, const std::string &dns)
  : unit_test(name, msg)
  , dns_(dns)
{
  add_test("open_close", std::bind(&ConnectionTestUnit::test_open_close, this), "open sql test");
  add_test("reopen", std::bind(&ConnectionTestUnit::test_reopen, this), "reopen sql test");
}

ConnectionTestUnit::~ConnectionTestUnit()
{}

void ConnectionTestUnit::test_open_close()
{
  matador::connection conn(connection_string());

  UNIT_ASSERT_FALSE(conn.is_open(), "connection must not be open");

  conn.open();

  UNIT_ASSERT_TRUE(conn.is_open(), "couldn't open sql sql");

  conn.close();

  UNIT_ASSERT_FALSE(conn.is_open(), "couldn't close sql sql");
}

void ConnectionTestUnit::test_reopen()
{
  matador::connection conn(connection_string());

  UNIT_ASSERT_FALSE(conn.is_open(), "connection must not be open");

  conn.open();

  UNIT_ASSERT_TRUE(conn.is_open(), "couldn't open sql sql");

  conn.close();

  conn.open();

  UNIT_ASSERT_TRUE(conn.is_open(), "couldn't open sql sql");

  conn.close();

  UNIT_ASSERT_FALSE(conn.is_open(), "couldn't close sql sql");
}

std::string ConnectionTestUnit::connection_string()
{
  return dns_;
}
