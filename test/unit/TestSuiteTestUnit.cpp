//
// Created by sascha on 12/23/16.
//

#include "TestSuiteTestUnit.hpp"
#include "FirstTestUnit.hpp"
#include "SecondTestUnit.hpp"

bool TestSuiteTestUnit::test_create()
{
  oos::test_suite ts;

  const oos::test_suite::summary &smry = ts.test_summary();
  bool result = smry.asserts == 0;
  result &= smry.errors == 0;
  result &= smry.failures == 0;
  result &= smry.succeeded == 0;
  result &= smry.tests == 0;
  return result;
}

bool TestSuiteTestUnit::test_list()
{
  oos::test_suite ts;

  bool result = ts.size() == 0;
  result &= ts.empty();

  return result;
}

bool TestSuiteTestUnit::test_init()
{
  char *argv1[] = {
    (char*)"",
    (char*)"list"
  };

  oos::test_suite ts;

  ts.init(2, argv1);

  const oos::test_suite::test_suite_args &args = ts.test_args();

  bool result = args.cmd == oos::test_suite::LIST;
  result &= args.initialized;
  result &= !args.brief;
  result &= args.unit_args.empty();

  char *argv2[] = {
    (char*)"",
    (char*)"list",
    (char*)"brief"
  };

  ts.init(3, argv2);

  result &= args.cmd == oos::test_suite::LIST;
  result &= args.initialized;
  result &= args.brief;
  result &= args.unit_args.empty();

  char *argv3[] = {
    (char*)"",
    (char*)"exec",
    (char*)"all"
  };

  ts.init(3, argv3);

  result &= args.cmd == oos::test_suite::EXECUTE;
  result &= args.initialized;
  result &= args.unit_args.empty();

  return result;
}

bool TestSuiteTestUnit::test_method()
{
  oos::test_suite ts;
  ts.register_unit(new FirstTestUnit);
  ts.register_unit(new SecondTestUnit);

  char *argv[] = {
    (char*)"",
    (char*)"exec",
    (char*)"first:sub1"
  };

  ts.init(3, argv);

  const oos::test_suite::test_suite_args &args = ts.test_args();

  bool result = args.cmd == oos::test_suite::EXECUTE;
  result &= args.initialized;
  result &= args.unit_args.size() == 1;

  auto &unit = args.unit_args.front();

  result &= unit.unit == "first";
  result &= unit.tests.size() == 1;

  ts.run();

  // Todo: check summary

  return result;
}

bool TestSuiteTestUnit::test_unit()
{
  oos::test_suite ts;
  ts.register_unit(new FirstTestUnit);
  ts.register_unit(new SecondTestUnit);

  char *argv[] = {
    (char*)"",
    (char*)"exec",
    (char*)"first"
  };

  ts.init(3, argv);

  const oos::test_suite::test_suite_args &args = ts.test_args();

  bool result = args.cmd == oos::test_suite::EXECUTE;
  result &= args.initialized;
  result &= args.unit_args.size() == 1;

  ts.run();

  // Todo: check summary

  return result;
}

bool TestSuiteTestUnit::test_suite()
{
  oos::test_suite ts;
  ts.register_unit(new FirstTestUnit);
  ts.register_unit(new SecondTestUnit);

  char *argv[] = {
    (char*)"",
    (char*)"exec",
    (char*)"all"
  };

  ts.init(3, argv);

  const oos::test_suite::test_suite_args &args = ts.test_args();

  bool result = args.cmd == oos::test_suite::EXECUTE;
  result &= args.initialized;
  result &= args.unit_args.empty();

  ts.run();

  // Todo: check summary

  return result;
}
