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

#include "object/object_store.hpp"

#include "unit/unit_test.hpp"

namespace oos {
class session;
}

class SessionTestUnit : public oos::unit_test
{
public:
  SessionTestUnit(const std::string &name, const std::string &msg, const std::string &db);
  virtual ~SessionTestUnit();

  virtual void initialize();
  virtual void finalize();

  void test_open_close();
  void test_create_drop();
  void test_drop();
  void test_reopen();

protected:
  oos::session* create_session();

private:
  oos::object_store ostore_;
  std::string db_;
  oos::session *session_;
};

#endif /* SESSION_TEST_UNIT_HPP */