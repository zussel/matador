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

#include "unit/unit_test.hpp"
#include "unit/test_suite.hpp"

#include <tr1/functional>

using namespace oos;

class FirstTestUnit : public unit_test
{
public:
  FirstTestUnit() : unit_test("First test unit")
  {
    add_test(std::tr1::bind(&FirstTestUnit::first_sub_test, this), "sub first");
    add_test(std::tr1::bind(&FirstTestUnit::second_sub_test, this), "sub second");
    add_test(std::tr1::bind(&FirstTestUnit::third_sub_test, this), "sub third");
  }
  virtual ~FirstTestUnit() {}
  
  void first_sub_test()
  {
    assert_equal(1, 1, "one is one");
    
    assert_equal(1, 0, "one is not zero");
  }
  void second_sub_test()
  {
    // gives warning
    warning();
  }
  void third_sub_test()
  {
  }
  /**
   * Initializes a test unit
   */
  virtual void initialize() {}
  virtual void finalize() {}
};

class SecondTestUnit : public unit_test
{
public:
  SecondTestUnit() : unit_test("Second test unit")
  {
    add_test(std::tr1::bind(&SecondTestUnit::small_test, this), "sub small");
    add_test(std::tr1::bind(&SecondTestUnit::big_test, this), "sub big");
  }
  virtual ~SecondTestUnit() {}

  void small_test()
  {
  }
  void big_test()
  {
  }
  /**
   * Initializes a test unit
   */
  virtual void initialize() {}
  virtual void finalize() {}
};

int main(int /*argc*/, char */*argv*/[])
{
  test_suite::instance().register_unit(new FirstTestUnit());
  test_suite::instance().register_unit(new SecondTestUnit());
  
  test_suite::instance().run();
}
