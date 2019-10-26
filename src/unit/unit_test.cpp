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

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <cstring>

using namespace std::chrono;

namespace matador {

unit_test::unit_test(std::string name, std::string caption)
  : name_(std::move(name))
  , caption_(std::move(caption))
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

bool unit_test::execute(bool quiet)
{
  // execute each test
  bool succeeded = true;
  std::for_each(test_func_infos_.begin(), test_func_infos_.end(), [&](t_test_func_info_vector::value_type &info) {
    execute(info, quiet);
    if (succeeded && !info.succeeded) {
      succeeded = false;
    }
  });
  return succeeded;
}

bool unit_test::execute(const std::string &test, bool quiet)
{
  auto i = std::find_if(test_func_infos_.begin(), test_func_infos_.end(), [test](const t_test_func_info_vector::value_type &x) {
    return x.name == test;
  });

  if (i == test_func_infos_.end()) {
    if (!quiet) {
      std::cout << "couldn't find test [" << test << "] of unit [" << caption_ << "]\n";
    }
    return false;
  } else {
    execute(*i, quiet);
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

void unit_test::assert_true(bool a, int line, const char *file)
{
  ++current_test_func_info->assertion_count;
  if (!a) {
    std::stringstream msgstr;
    msgstr << "FAILURE at " << file << ":" << line << ": expected value to be true";
    throw unit_exception(msgstr.str());
  }
}

void unit_test::assert_false(bool a, int line, const char *file)
{
  ++current_test_func_info->assertion_count;
  if (a) {
    std::stringstream msgstr;
    msgstr << "FAILURE at " << file << ":" << line << ": expected value to be false";
    throw unit_exception(msgstr.str());
  }
}

void unit_test::expect_true(bool a, int line, const char *file)
{
  ++current_test_func_info->error_count;
  if (!a) {
    ++current_test_func_info->errors;
    std::cout << "ERROR at " << file << ":" << line << ": expected value to be true";
  }
}

void unit_test::expect_false(bool a, int line, const char *file)
{
  ++current_test_func_info->error_count;
  if (a) {
    ++current_test_func_info->errors;
    std::cout << "ERROR at " << file << ":" << line << ": expected value to be false";
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

void unit_test::execute(test_func_info &test_info, bool quiet)
{
  initialize();
  if (!quiet) {
    std::cout << std::left << std::setw(70) << test_info.caption << " ... " << std::flush;
  }

  long dur(0L);
  try {
    current_test_func_info = &test_info;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    test_info.func();
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    dur = duration_cast<microseconds>( t2 - t1 ).count();

  } catch (unit_exception &ex) {
    test_info.succeeded = false;
    test_info.message = ex.what();
  } catch (std::exception &ex) {
    test_info.succeeded = false;
    test_info.message = ex.what();
  }
  finalize();
  if (quiet) {
    return;
  }
  if (test_info.succeeded) {
    std::cout << "PASS (" << ::std::right << std::setw(3) << test_info.assertion_count + test_info.error_count << " assertions) (" << std::setw(8) << (double)(dur)/1000.0 << "ms)\n";
  } else {
    std::cout << "FAILED\n\t" << test_info.message << "\n";
  }
}

void unit_test::assert_equal(char *&a, const char *&b, int line, const char *file) {
  ++current_test_func_info->assertion_count;
  if (strcmp(a, b) != 0) {
    std::stringstream msgstr;
    msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b;
    throw unit_exception(msgstr.str());
  }
}

void unit_test::assert_equal(const std::string &a, const char *b, int line, const char *file) {
  ++current_test_func_info->assertion_count;
  if (strcmp(a.c_str(), b) != 0) {
    std::stringstream msgstr;
    msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b;
    throw unit_exception(msgstr.str());
  }
}

void unit_test::assert_equal(const char *a, const std::string &b, int line, const char *file) {
  ++current_test_func_info->assertion_count;
  if (strcmp(a, b.c_str()) != 0) {
    std::stringstream msgstr;
    msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b;
    throw unit_exception(msgstr.str());
  }
}

void unit_test::assert_equal(const char *a, const char *b, int line, const char *file) {
  ++current_test_func_info->assertion_count;
  if (strcmp(a, b) != 0) {
    std::stringstream msgstr;
    msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b;
    throw unit_exception(msgstr.str());
  }
}

void unit_test::assert_equal(const bool &a, const bool &b, int line, const char *file) {
  ++current_test_func_info->assertion_count;
  if (a != b) {
    std::stringstream msgstr;
    msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b;
    throw unit_exception(msgstr.str());
  }
}

void unit_test::expect_equal(const double &a, const double &b, int line, const char *file) {
  ++current_test_func_info->error_count;
  if (std::abs(a - b) > 0.000001) {
    ++current_test_func_info->errors;
    std::cout << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b;
  }
}

void unit_test::expect_equal(const char *a, const std::string &b, int line, const char *file) {
  ++current_test_func_info->error_count;
  if (strcmp(a, b.c_str()) != 0) {
    ++current_test_func_info->errors;
    std::cout << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b;
  }
}

void unit_test::expect_equal(const std::string &a, const char *b, int line, const char *file) {
  ++current_test_func_info->error_count;
  if (strcmp(a.c_str(), b) != 0) {
    ++current_test_func_info->errors;
    std::cout << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b;
  }
}

void unit_test::expect_equal(const char *a, const char *b, int line, const char *file) {
  ++current_test_func_info->error_count;
  if (strcmp(a, b) != 0) {
    ++current_test_func_info->errors;
    std::cout << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b;
  }
}

}
