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

#ifndef DATABASE_TEST_UNIT_HPP
#define DATABASE_TEST_UNIT_HPP

#include "matador/object/object_store.hpp"

#include "matador/utils/time.hpp"

#include "matador/unit/unit_test.hpp"

namespace matador {
class session;
}

class DatabaseTestUnit : public matador::unit_test
{
public:
  DatabaseTestUnit(const std::string &name, const std::string &msg, const std::string &db = "memory", const matador::time &timeval = matador::time(2015, 3, 15, 13, 56, 23, 123));
  virtual ~DatabaseTestUnit();

  virtual void initialize();
  virtual void finalize();

  void test_datatypes();
  void test_primary_key();
  void test_insert();
  void test_update();
  void test_delete();
  void test_reload_simple();
  void test_reload();
  void test_reload_container();
  void test_reload_relation();

protected:
  matador::session* create_session();

  matador::object_store& ostore();
  const matador::object_store& ostore() const;

  std::string db() const;

private:
  matador::object_store ostore_;
  std::string db_;
  matador::session *session_;
  matador::time time_val_;
};

#endif /* DATABASE_TEST_UNIT_HPP */
