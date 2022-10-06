#ifndef TEST_SUITE_HPP
#define TEST_SUITE_HPP

#ifdef _MSC_VER
  #ifdef matador_unit_EXPORTS
    #define OOS_UNIT_API __declspec(dllexport)
    #define EXPIMP_UNIT_TEMPLATE
  #else
    #define OOS_UNIT_API __declspec(dllimport)
    #define EXPIMP_UNIT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_UNIT_API
#endif

#include "matador/unit/unit_test.hpp"

#include <memory>
#include <map>
#include <string>
#include <vector>

/**
 * @namespace matador
 * @brief The Open Object Store namespace.
 *
 * This is the namespace where all OOS related classes,
 * function and code should be put into.
 */
namespace matador {

class unit_test;

/**
 * @class test_suite
 * @brief The container for all unit tests.
 * 
 * The test_suite class is the container for all test_unit
 * classes. It is a singleton, so only one instance exists.
 * It can execute all test_units or a specific test_unit.
 * It also provides function listing all test_unit classes
 * and their tests.
 */
class OOS_UNIT_API test_suite
{
public:
  /**
   * @brief Holds the summary result of the executed tests
   */
  struct summary {
    /**
     * @brief Reset the summary data
     */
    void reset()
    {
      asserts = 0;
      failures = 0;
      errors = 0;
    }

    /**
     * @brief Evaluate the test info data
     *
     * Once a test was executed an info struct is filled
     * with the result. This method syncs these infos
     * with overall summary.
     *
     * @param info The test info struct
     */
    void evaluate(const unit_test::test_func_info &info)
    {
      ++tests;
      asserts += (info.assertion_check_count + info.error_check_count);
      errors += info.error_count;
      if (info.succeeded) {
        ++succeeded;
      } else {
        ++failures;
      }
    }

    /**
     * @brief Outputs The summary struct to on std::ostream
     *
     * @param out The stream to write on
     * @param s The summary object
     * @return Reference to the stream
     */
    friend OOS_UNIT_API std::ostream& operator<<(std::ostream& out, const test_suite::summary &s);

    /// @cond MATADOR_DEV
    size_t tests = 0;
    size_t asserts = 0;
    size_t succeeded = 0;
    size_t failures = 0;
    size_t errors = 0;
    /// @endcond
  };

  /**
   * @brief test_suite commands
   *
   * This is an enumeration for
   * all test_suite commands
   */
  enum test_suite_cmd
  {
    UNKNOWN = 0, /**<Enum type for an unknown test_suite command. */
    LIST,        /**<Enum type for the list command. */
    EXECUTE      /**<Enum type for the execute command. */
  };

  typedef std::vector<std::string> t_string_vector; /**< Shortcut for a string vector */

  /**
   * Test suite test unit arguments struct
   */
  struct test_unit_args
  {
    std::string unit;       /**< Test unit name */
    t_string_vector tests; /**< Vector of test names contained by the unit test */
  };

  /**
   * Test suite arguments struct
   */
  struct test_suite_args
  {
    test_suite_cmd cmd = UNKNOWN; /**< Test suite command to be executed */
    bool initialized = false;     /**< Indicates wether the suite is initialized or not */
    bool brief = false;           /**< If true for LIST command only a brief list is printed */
    bool quiet = false;           /**< If true no output is written to stdout */
    std::vector<test_unit_args> unit_args; /**< Vector of test unit arguments */
  };

private:

  typedef std::shared_ptr<unit_test> unit_test_ptr;
  typedef std::map<std::string, unit_test_ptr> t_unit_test_map;
  typedef t_unit_test_map::value_type value_type;

  struct unit_executer
  {
    unit_executer(summary &s, bool q);
    void operator()(test_suite::value_type &x);
    
    bool succeeded;
    bool quiet;
    summary &summary_;
  };

  struct unit_lister
  {
    explicit unit_lister(std::ostream &o, bool b = false);
    void operator()(const test_suite::value_type &x) const;
    std::ostream &out;
    bool brief;
  };

public:
  test_suite() = default;
  virtual ~test_suite();
  
  /**
   * @brief Initialize test_suite
   *
   * Reads, parses and initializes the
   * test_suite.
   *
   * @param argc Number of arguments.
   * @param argv List of arguments.
   */
  void init(int argc, char *argv[]);

  /**
   * @brief Register a new test_unit serializable.
   *
   * Registers a new test_unit serializable identified
   * by the given name.
   *
   * @param utest test_unit serializable.
   */
  void register_unit(unit_test *utest);

  /**
   * @brief Executes the test_suite.
   *
   * Executes all test unit classes or the
   * command given via init.
   * 
   * @return True if all tests succeeded.
   */
  bool run();

  /**
   * @brief Executes a specific test_unit.
   *
   * Executes the test_unit with the given name.
   * If there is no test_unit serializable with such a
   * name a message is displayed.
   *
   * @param unit The name of the test_unit serializable to execute.
   * @return True if all tests succeeded.
   */
  bool run(const std::string &unit);

  /**
   * @brief Executes a specific test_unit.
   *
   * Executes the test_unit with the given args.
   * Arguments contain name of test unit and
   * a list of all tests to be executed.
   * If list is empty all tests are executed.
   * If there is no test_unit serializable with such a
   * name a message is displayed.
   *
   * @param unit_args The arguments of the test_unit to execute.
   * @return True if all tests succeeded.
   */
  bool run(const test_unit_args &unit_args);

  /**
   * @brief Executes a single test of a test_unit.
   *
   * Executes a single test of a test_unit serializable,
   * identified by the name of the test and the name
   * of the test_unit serializable.
   * If test couldn't be found a message is displayed.
   *
   * @param unit Name of the test_unit serializable.
   * @param test Name of the test to execute.
   * @return True if test succeeded.
   */
  bool run(const std::string &unit, const std::string &test);

  /**
   * Returns the number of unit test classes
   *
   * @return Number of unit test classes
   */
  std::size_t size() const;

  /**
   * Returns true if test suite is empty
   *
   * @return True if test suite is empty
   */
  bool empty() const;

  /**
   * Returns the begin iterator of the unit test map
   *
   * @return Begin iterator of the unit test map
   */
  t_unit_test_map::const_iterator begin() const;

  /**
   * Returns the end iterator of the unit test map
   *
   * @return End iterator of the unit test map
   */
  t_unit_test_map::const_iterator end() const;

  /**
   * Return the current summary for the test suite
   *
   * @return Current summary for the test suite
   */
  const summary& test_summary() const;

  /**
   * Returns test suites configurated arguments
   *
   * @return Test suites configurated arguments
   */
  const test_suite_args& test_args() const;

  /**
   * Disables any output
   *
   * @param q If true a output is disabled
   */
  void quiet(bool q = true);

private:
  test_suite_args args_;
  t_unit_test_map unit_test_map_;
  summary summary_;
};

}

#endif /* TEST_SUITE_HPP */
