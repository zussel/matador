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

#ifndef TEST_SUITE_HPP
#define TEST_SUITE_HPP

#include "tools/singleton.hpp"

#include <memory>
#include <map>
#include <string>

namespace oos {

class unit_test;

class test_suite : public singleton<test_suite>
{
  friend class singleton<test_suite>;

  typedef enum test_suite_cmd_enum
  {
    UNKNOWN = 0,
    LIST,
    EXECUTE
  } test_suite_cmd;

  typedef struct test_suite_args_struct
  {
    test_suite_args_struct()
      : cmd(UNKNOWN), initialized(false)
    {}
    test_suite_cmd cmd;
    bool initialized;
    std::string unit;
    std::string test;
  } test_suite_args;

  test_suite();

public:
  typedef std::tr1::shared_ptr<unit_test> unit_test_ptr;
  typedef std::map<std::string, unit_test_ptr> t_unit_test_map;
  typedef t_unit_test_map::value_type value_type;

public:
  virtual ~test_suite();
  
  void init(int argc, char *argv[]);
  void register_unit(const std::string &name, unit_test *utest);
  // executes all test unit classes
  // or what call to init has set up
  // via command args
  void run();
  // execures one test unit class
  void run(const std::string &unit);
  // execute one test of a test unit class
  void run(const std::string &unit, const std::string &test);

private:
  test_suite_args args_;
  t_unit_test_map unit_test_map_;
};

}

#endif /* TEST_SUITE_HPP */
