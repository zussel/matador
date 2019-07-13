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

#include "matador/unit/unit_test.hpp"
#include "matador/unit/test_suite.hpp"
#include "matador/unit/unit_exception.hpp"

#include <functional>

class FirstTestUnit : public matador::unit_test
{
public:
  FirstTestUnit() : unit_test("first", "First test unit")
  {
    add_test("sub1", std::bind(&FirstTestUnit::first_sub_test, this), "sub first");
    add_test("sub2", std::bind(&FirstTestUnit::second_sub_test, this), "sub second");
    add_test("sub3", std::bind(&FirstTestUnit::third_sub_test, this), "sub third");
  }

  void first_sub_test()
  {
    UNIT_ASSERT_EQUAL(1, 1);
    
    try {
      UNIT_ASSERT_EQUAL(1, 0);
    } catch (matador::unit_exception &) {
      UNIT_ASSERT_TRUE(true);
    }
  }
  void second_sub_test()
  {
    // gives warning
    //UNIT_WARN("this is a test warning!");
    UNIT_ASSERT_TRUE(true);
  }
  void third_sub_test()
  {
    UNIT_ASSERT_TRUE(true);
  }
};
