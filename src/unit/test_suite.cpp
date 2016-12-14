#include "oos/unit/test_suite.hpp"
#include "oos/unit/unit_test.hpp"

#include <algorithm>
#include <functional>
#include <iostream>

namespace oos {

test_suite::test_suite()
{}

void test_suite::register_unit(unit_test *utest)
{
  unit_test_map_.insert(std::make_pair(utest->name(), unit_test_ptr(utest)));
}

test_suite::unit_executer::unit_executer(summary &sum)
  : succeeded(true)
  , summary_(sum)
{}

void test_suite::unit_executer::operator()(test_suite::value_type &x)
{
  std::cout << "[" << x.second->caption() << "]\n";
  bool result = x.second->execute();
  std::for_each(x.second->test_func_infos_.begin(), x.second->test_func_infos_.end(), [this](const unit_test::test_func_info &info) {
    summary_.evaluate(info);
  });
//  summary_.evaluate(result);
  if (succeeded && !result) {
    succeeded = result;
  }
}

test_suite::unit_lister::unit_lister(std::ostream &o, bool b)
  : out(o), brief(b)
{}

void test_suite::unit_lister::operator()(const test_suite::value_type &x) const
{
  if (!brief) {
    out << "Unit Test [" << x.first << "] has the following test:\n";
  }

  const unit_test::t_test_func_info_vector &tests = x.second->test_func_infos_;
  std::for_each(tests.begin(), tests.end(), [this](const unit_test::t_test_func_info_vector::value_type &x) {
    if (brief) {
      out << x.name << ":" << x.caption << "\n";
    } else {
      out << "Test [" << x.name << "]: " << x.caption << std::endl;
    }
  });
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
      std::stringstream sval(val);
      std::string part;
      while (std::getline(sval, part, ',')) {
        size_t pos = part.find(':');

        test_unit_args unit_args;

        if (pos == std::string::npos) {
          // execute all test of a unit class
          unit_args.unit = part;
        } else {
          // extract unit and test name
          unit_args.unit = part.substr(0, pos);
          std::stringstream tests(part.substr(pos+1));
          std::string test;
          while (std::getline(tests, test, ':')) {
            unit_args.tests.push_back(test);
          }
        }

        args_.unit_args.push_back(unit_args);
      }
    }
  } else {
    return;
  }
  args_.initialized = true;
}

bool test_suite::run()
{
  if (args_.initialized) {
    switch (args_.cmd) {
      case LIST:
        std::for_each(unit_test_map_.begin(), unit_test_map_.end(), unit_lister(std::cout, args_.brief));
        break;
      case EXECUTE:
        summary_.reset();
        if (!args_.unit_args.empty()) {
          bool result = true;
          for (auto item : args_.unit_args) {
            bool succeeded = run(item);
            if (result && !succeeded) {
              result = succeeded;
            }
          }
          std::cout << summary_;
          return result;
        } else {
          unit_executer ue(summary_);
          std::for_each(unit_test_map_.begin(), unit_test_map_.end(), std::ref(ue));
          std::cout << summary_;
          return ue.succeeded;
        }
      default:
        break;
      }
  } else {
    std::cout << "usage: test_oos [list]|[exec <val>]\n";
  }
  return true;
}

bool test_suite::run(const std::string &unit)
{
  t_unit_test_map::const_iterator i = unit_test_map_.find(unit);
  if (i == unit_test_map_.end()) {
    std::cout << "couldn't find test unit [" << unit << "]\n";
    return false;
  } else {
    std::cout << "[" << i->second->caption() << "]\n";
    bool succeeded = i->second->execute();
    std::for_each(i->second->test_func_infos_.begin(), i->second->test_func_infos_.end(), [this](const unit_test::test_func_info &info) {
      summary_.evaluate(info);
    });
    return succeeded;
  }
}

bool test_suite::run(const test_unit_args &unit_args)
{
  bool result = true;
  if (unit_args.tests.empty()) {
    result = run(unit_args.unit);
  } else {
    for (auto test : unit_args.tests) {
      bool succeeded = run(unit_args.unit, test);
      if (result && !succeeded) {
        result = succeeded;
      }
    }
  }
  return result;
}

bool test_suite::run(const std::string &unit, const std::string &test)
{
  t_unit_test_map::const_iterator i = unit_test_map_.find(unit);
  if (i == unit_test_map_.end()) {
    std::cout << "couldn't find test unit [" << unit << "]\n";
    return false;
  } else {
    bool succeeded = i->second->execute(test);
    auto j = std::find_if(i->second->test_func_infos_.begin(), i->second->test_func_infos_.end(), [test](const unit_test::t_test_func_info_vector::value_type &x) {
      return x.name == test;
    });
    if (j != i->second->test_func_infos_.end()) {
      summary_.evaluate(*j);
    }
    return succeeded;
  }
}

std::ostream& operator<<(std::ostream& out, const test_suite::summary &s)
{
  out << "summary for " << s.tests << " tests with " << s.asserts << " asserts: (succeeded: " << s.succeeded << "), (failures: " << s.failures << "), (errors: " << s.errors << ")\n";
  return out;
}


}
