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

#include <iostream>

unit_test::unit_test(const std::string &caption)
 : caption_(caption)
{}

unit_test::~unit_test()
{
  test_func_info_list_.clear();
}

std::string unit_test::caption() const
{
  return caption_;
}

void unit_test::execute()
{
  t_test_func_info_list::iterator first = test_func_info_list_.begin();
  t_test_func_info_list::iterator last = test_func_info_list_.end();
  while (first != last) {
    initialize();
    test_func_info &info = *first++;
    std::cout << "Executing test [" << info.caption << "] ... ";
    try {
      info.func();
    } catch (unit_exception &ex) {
      info.succeeded = false;
      info.message = ex.what();
    }
    finalize();
    if (info.succeeded) {
      std::cout << "succeeded\n";
    } else {
      std::cout << "failed: " << info.message << "\n";
    }
  }
  // execute each test
}

void unit_test::add_test(const test_func &test, const std::string &caption)
{
  test_func_info_list_.push_back(test_func_info(test, caption));
}

void unit_test::assert_true(bool a, const std::string &msg)
{
  if (!a) {
    // throw exception
      std::stringstream msgstr;
      msgstr << "FAILURE: value " << a << " is false: " << msg;
      throw unit_exception(msgstr.str());
  }
}

void unit_test::assert_false(bool a, const std::string &msg)
{
  if (a) {
    // throw exception
      std::stringstream msgstr;
      msgstr << "FAILURE: value " << a << " is true: " << msg;
      throw unit_exception(msgstr.str());
  }
}

void unit_test::warning()
{
}
