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

#ifndef TRANSACTION_TEST_UNIT_HPP
#define TRANSACTION_TEST_UNIT_HPP

#include "object/object_store.hpp"

#include "unit/unit_test.hpp"

namespace oos {
class session;
}

class TransactionTestUnit : public oos::unit_test
{
public:
  TransactionTestUnit(const std::string &name, const std::string &msg, const std::string &dns = "memory");
  virtual ~TransactionTestUnit();

  void test_simple();
  void test_nested();
  void test_foreign();
  void test_has_many_list_commit();
  void test_has_many_list();
//  void test_with_vector();

protected:
  std::string connection_string();

private:
  std::string dns_;

};

#endif /* TRANSACTION_TEST_UNIT_HPP */
