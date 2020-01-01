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

#ifndef SESSION_TEST_UNIT_HPP
#define SESSION_TEST_UNIT_HPP

#include "matador/unit/unit_test.hpp"

namespace matador {
class session;
}

class ConnectionTestUnit : public matador::unit_test
{
public:
  ConnectionTestUnit(const std::string &prefix, std::string dns);
  ~ConnectionTestUnit() override = default;

  void test_open_close();
  void test_reopen();
  void test_reconnect();
  void test_connection_failed();

protected:
  std::string connection_string();

private:
  std::string dns_;
  std::string db_vendor_;
};

#endif /* SESSION_TEST_UNIT_HPP */