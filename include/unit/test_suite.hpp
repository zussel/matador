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

#include <tr1/memory>
#include <list>

class unit_test;

class test_suite : public singleton<test_suite>
{
  friend class singleton<test_suite>;
  
public:
  typedef std::tr1::shared_ptr<unit_test> unit_test_ptr;
  typedef std::list<unit_test_ptr> t_unit_test_list;

public:
  virtual ~test_suite();
  
  void register_unit(unit_test *utest);
  void run();

private:
  t_unit_test_list unit_test_list_;
};

#endif /* TEST_SUITE_HPP */
