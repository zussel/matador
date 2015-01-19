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

#include "tools/varchar.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>

namespace oos {

unit_test::unit_test(const std::string &name, const std::string &caption)
 : name_(name)
 , caption_(caption)
 , current_test_func_info(nullptr)
{}

unit_test::~unit_test()
{
  test_func_infos_.clear();
}

std::string unit_test::name() const
{
  return name_;
}

std::string unit_test::caption() const
{
  return caption_;
}

bool unit_test::execute()
{
  // execute each test
  bool succeeded = true;
  std::for_each(test_func_infos_.begin(), test_func_infos_.end(), [&](t_test_func_info_vector::value_type &info) {
    execute(info);
    if (succeeded && !info.succeeded) {
      succeeded = false;
    }
  });
  return succeeded;
}

bool unit_test::execute(const std::string &test)
{
  t_test_func_info_vector::iterator i = std::find_if(test_func_infos_.begin(), test_func_infos_.end(), [test](const t_test_func_info_vector::value_type &x) {
    return x.name == test;
  });

  if (i == test_func_infos_.end()) {
    std::cout << "couldn't find test [" << test << "] of unit [" << caption_ << "]\n";
    return false;
  } else {
    execute(*i);
    return i->succeeded;
  }
}

void unit_test::list(std::ostream &out) const
{
  std::for_each(test_func_infos_.begin(), test_func_infos_.end(), [&](const t_test_func_info_vector::value_type &info) {
    out << "Test [" << info.name << "]: " << info.caption << std::endl;
  });
}

void unit_test::add_test(const std::string &name, const test_func &test, const std::string &caption)
{
  test_func_infos_.push_back(test_func_info(test, name, caption));
}

void unit_test::assert_true(bool a, const std::string &msg, int line, const char *file)
{
  ++current_test_func_info->assertion_count;
  if (!a) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is false: " << msg;
      throw unit_exception(msgstr.str());
  }
}

void unit_test::assert_false(bool a, const std::string &msg, int line, const char *file)
{
  ++current_test_func_info->assertion_count;
  if (a) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is true: " << msg;
      throw unit_exception(msgstr.str());
  }
}

void unit_test::error(const std::string &msg, int line, const char *file)
{
  std::stringstream msgstr;
  msgstr << "FAILURE at " << file << ":" << line << ": " << msg;
  throw unit_exception(msgstr.str());
}

void unit_test::warn(const std::string &msg, int line, const char *file)
{
  std::cout << "WARNING at " << file << ":" << line << ": " << msg;
}

void unit_test::info(const std::string &msg)
{
  std::cout << "INFO: " << msg;
}

void unit_test::execute(test_func_info &test_info)
{
    initialize();
    std::cout << std::left << std::setw(70) << test_info.caption << " ... " << std::flush;
    try {
      current_test_func_info = &test_info;
      test_info.func();
    } catch (unit_exception &ex) {
      test_info.succeeded = false;
      test_info.message = ex.what();
    } catch (std::exception &ex) {
      test_info.succeeded = false;
      test_info.message = ex.what();
    }
    finalize();
    if (test_info.succeeded) {
      std::cout << "PASS (" << test_info.assertion_count << " assertions)\n";
    } else {
      std::cout << "FAILED\n\t" << test_info.message << "\n";
    }
}

void unit_test::reset() {

}

}
