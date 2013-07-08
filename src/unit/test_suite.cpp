#include "unit/test_suite.hpp"
#include "unit/unit_test.hpp"

#include <algorithm>
#include <functional>
#include <iostream>

namespace oos {

test_suite::test_suite()
{}

void test_suite::register_unit(const std::string &name, unit_test *utest)
{
  unit_test_map_.insert(std::make_pair(name, unit_test_ptr(utest)));
}

test_suite::unit_executer::unit_executer()
{}

void test_suite::unit_executer::operator()(test_suite::value_type &x)
{
  std::cout << "Executing test unit [" << x.second->caption() << "]\n";
  x.second->execute();
}

test_suite::unit_lister::unit_lister(std::ostream &o, bool b, bool c)
  : out(o), brief(b), cmake(c)
{}

void test_suite::unit_lister::operator()(test_suite::value_type &x)
{
  if (brief) {
    x.second->list(out, x.first);
  } else if (cmake) {
    
  } else {
    out << "Unit Test [" << x.first << "] has the following test:\n";
    x.second->list(out);
  }
}

test_suite::~test_suite()
{
  unit_test_map_.clear();
}

void test_suite::init(int argc, char *argv[])
{
  // check for
  if (argc < 2) {
    // no arguments
    return;
  }
  std::string arg(argv[1]);
  
  if (arg == "list") {
    // list all test units
    args_.cmd = LIST;
    if (argc == 3) {
      if (strcmp(argv[2], "brief") == 0) {
        args_.brief = true;
      } else if (strcmp(argv[2] , "cmake") == 0) {
        args_.cmake = true;
      } else {
        // unknow option arg
      }
    } else {
    }
  } else if (arg == "exec") {
    if (argc < 3) {
      return;
    }

    args_.cmd = EXECUTE;

    std::string val(argv[2]);
    if (val == "all") {
    } else {
      size_t pos = val.find(':');
      
      if (pos == std::string::npos) {
        // execute all test of a unit class
        args_.unit = val;
      } else {
        // extract unit and test name
        args_.unit = val.substr(0, pos);
        args_.test = val.substr(pos+1);
      }
    }
  } else {
    return;
  }
  args_.initialized = true;
}

void test_suite::run()
{
  if (args_.initialized) {
    switch (args_.cmd) {
      case LIST:
        std::for_each(unit_test_map_.begin(), unit_test_map_.end(), unit_lister(std::cout, args_.brief, args_.cmake));
        break;
      case EXECUTE:
        if (!args_.unit.empty() && !args_.test.empty()) {
          run(args_.unit, args_.test);
        } else if (!args_.unit.empty()) {
          run(args_.unit);
        } else {
          std::for_each(unit_test_map_.begin(), unit_test_map_.end(), unit_executer());
        }
        break;
      default:
        break;
      }
  } else {
    std::cout << "usage: test_oos [list]|[exec <val>]\n";
  }
}

void test_suite::run(const std::string &unit)
{
  t_unit_test_map::const_iterator i = unit_test_map_.find(unit);
  if (i == unit_test_map_.end()) {
    std::cout << "couldn't find test unit [" << unit << "]\n";
  } else {
    i->second->execute();
  }
}

void test_suite::run(const std::string &unit, const std::string &test)
{
  t_unit_test_map::const_iterator i = unit_test_map_.find(unit);
  if (i == unit_test_map_.end()) {
    std::cout << "couldn't find test unit [" << unit << "]\n";
  } else {
    i->second->execute(test);
  }
}

}
