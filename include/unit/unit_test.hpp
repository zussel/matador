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

#ifndef UNIT_TEST_HPP
#define UNIT_TEST_HPP

#include "unit/unit_exception.hpp"

#include <tr1/functional>
#include <list>
#include <string>
#include <sstream>

class unit_test
{
public:
  typedef std::tr1::function<void ()> test_func;

  unit_test(const std::string &caption);
  virtual ~unit_test();
  
  /**
   * Initializes a test unit
   */
  virtual void initialize() = 0;
  virtual void finalize() = 0;

  std::string caption() const;

  void execute();
  void add_test(test_func test);

  template < class T >
  void assert_equal(T a, T b, const std::string &msg)
  {
    if (a != b) {
      // throw exception
      std::stringstream msgstr;
      msgstr << "FAILURE: value " << a << " is not equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }
  template < class T >
  void assert_not_equal(T a, T b, const std::string &msg)
  {
    if (a == b) {
      // throw exception
      std::stringstream msgstr;
      msgstr << "FAILURE: value " << a << " is equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }
  template < class T >
  void assert_null(T *a, const std::string &msg)
  {
    if (a != 0) {
      // throw exception
      std::stringstream msgstr;
      msgstr << "FAILURE: value " << a << " is not null: " << msg;
      throw unit_exception(msgstr.str());
    }
  }
  template < class T >
  void assert_not_null(T *a, const std::string &msg)
  {
    if (a == 0) {
      // throw exception
      std::stringstream msgstr;
      msgstr << "FAILURE: value " << a << " is null: " << msg;
      throw unit_exception(msgstr.str());
    }
  }
  void assert_true(bool a, const std::string &msg);
  void assert_false(bool a, const std::string &msg);
  void warning();

private:
  std::string caption_;

  typedef std::list<test_func> t_test_func_list;
  t_test_func_list test_func_list_;
};

#endif /* UNIT_TEST_HPP */
