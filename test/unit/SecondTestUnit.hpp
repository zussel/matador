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

#include <functional>

class SecondTestUnit : public matador::unit_test
{
public:
  SecondTestUnit() : unit_test("second", "Second test unit")
  {
    add_test("small", std::bind(&SecondTestUnit::small_test, this), "sub small");
    add_test("big", std::bind(&SecondTestUnit::big_test, this), "sub big");
  }

  void small_test()
  {
    UNIT_ASSERT_TRUE(true);
  }
  void big_test()
  {
    UNIT_ASSERT_TRUE(true);
  }
};
