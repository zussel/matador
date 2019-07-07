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

#ifndef MATADOR_DOXYGEN_DOC
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
#endif /* MATADOR_DOXYGEN_DOC */

#include "matador/unit/unit_exception.hpp"

#include <functional>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <type_traits>
#include <iostream>

/**
 * @file unit_test.hpp
 * @brief Contains the unit_test class an global unit defines
 *
 * This file contains the declaration of
 * the unit_test class and the global unit
 * assert defines.
 */

/**
 * @brief Checks if a is equal b.
 *
 * If a is not equal b the test method throws
 * a unit_exception with the given message. The
 * exception is caught by the test_suite the
 * message is displayed
 */
#define UNIT_ASSERT_EQUAL(a, b, msg)     assert_equal(a, b, msg, __LINE__, __FILE__)

/**
 * @brief Checks if a is not msgstrequal b.
 *
 * If a is equal b the test method throws
 * a unit_exception with the given message. The
 * exception is caught by the test_suite the
 * message is displayed
 */
#define UNIT_ASSERT_NOT_EQUAL(a, b, msg) assert_not_equal(a, b, msg, __LINE__, __FILE__)

/**
 * @brief Checks if a is greater b.
 *
 * If a is not greater b the test method throws
 * a unit_exception with the given message. The
 * exception is caught by the test_suite the
 * message is displayed
 */
#define UNIT_ASSERT_GREATER(a, b, msg)     assert_greater(a, b, msg, __LINE__, __FILE__)

/**
 * @brief Checks if a is greater b.
 *
 * If a is not greater b the test method throws
 * a unit_exception with the given message. The
 * exception is caught by the test_suite the
 * message is displayed
 */
#define UNIT_ASSERT_LESS(a, b, msg)        assert_less(a, b, msg, __LINE__, __FILE__)

/**
 * @brief Checks if a evaluates to false.
 *
 * If a doesn't evaluates to false the test method
 * throws a unit_exception with the given message.
 * The exception is caught by the test_suite the
 * message is displayed
 */
#define UNIT_ASSERT_FALSE(a, msg)        assert_false(a, msg, __LINE__, __FILE__)

/**
 * @brief Checks if a evaluates to true.
 *
 * If a doesn't evaluates to true the test method
 * throws a unit_exception with the given message.
 * The exception is caught by the test_suite the
 * message is displayed
 */
#define UNIT_ASSERT_TRUE(a, msg)         assert_true(a, msg, __LINE__, __FILE__)

/**
 * @brief Checks if o is NULL.
 *
 * If o isn't NULL the test method throws a
 * unit_exception with the given message. The
 * exception is caught by the test_suite the
 * message is displayed
 */
#define UNIT_ASSERT_NULL(o, msg)         assert_null(o, msg, __LINE__, __FILE__)

/**
 * @brief Checks if o is not NULL.
 *
 * If o is NULL the test method throws a
 * unit_exception with the given message. The
 * exception is caught by the test_suite the
 * message is displayed
 */
#define UNIT_ASSERT_NOT_NULL(o, msg)     assert_not_null(o, msg, __LINE__, __FILE__)

/**
* @brief Checks if a is equal b.
*
* If a is not equal b the test method throws
* a unit_exception with the given message. The
* exception is caught by the test_suite the
* message is displayed
*/
#define UNIT_ASSERT_EXCEPTION(expression, exception_type, exception_msg, msg)     \
  do {                                                                            \
    bool exception_caught(false);                                                 \
    try {                                                                         \
      expression;                                                                 \
    } catch(exception_type &ex) {                                                 \
      assert_equal(ex.what(), exception_msg, msg, __LINE__, __FILE__); \
      exception_caught = true;                                                    \
    }                                                                             \
    if (!exception_caught) {                                                      \
      error(msg, __LINE__, __FILE__);                                             \
    }                                                                             \
  } while(false);


/**
 * @brief Checks if a is equal b.
 *
 * If a is not equal b the test prints
 * the given message to stdout.
 */
#define UNIT_EXPECT_EQUAL(a, b, msg)     expect_equal(a, b, msg, __LINE__, __FILE__)

/**
 * @brief Checks if a is greater b.
 *
 * If a is not greater b the test prints
 * the given message to stdout.
 */
#define UNIT_EXPECT_GREATER(a, b, msg)   expect_greater(a, b, msg, __LINE__, __FILE__)

/**
 * @brief Checks if a evaluates to false.
 *
 * If a doesn't evaluates to false the test method
 * prints the given message to stdout.
 */
#define UNIT_EXPECT_FALSE(a, msg)        expect_false(a, msg, __LINE__, __FILE__)

/**
 * @brief Checks if a evaluates to true.
 *
 * If a doesn't evaluates to true the test method
 * prints the given message to stdout.
 */
#define UNIT_EXPECT_TRUE(a, msg)         expect_true(a, msg, __LINE__, __FILE__)

/**
 * @brief Throws an error.
 *
 * The test method throws an exception
 * with the given error message and the current
 * file and line number information.
 */
#define UNIT_FAIL(msg)                   \
  do {                                   \
    std::stringstream fail_stream;       \
    fail_stream << msg;                  \
    error(fail_stream.str(), __LINE__, __FILE__);\
  } while (false)

/**
 * @brief Displays a warning.
 *
 * The test method displays a warning
 * with the given message and the current
 * file and line number information.
 */
#define UNIT_WARN(msg)                   \
  do {                                   \
    std::stringstream warn_stream;       \
    warn_stream << msg << "\n";          \
    warn(warn_stream.str(), __LINE__, __FILE__); \
  } while (false)

/**
 * @brief Displays an informatkon.
 *
 * The test method displays an information
 * with the given message.
 */
#define UNIT_INFO(msg)                   info(msg)

namespace matador {

/**
 * @class unit_test
 * @brief A unit_test consists of serveral tests.
 *
 * The unit_test class is the base class for all
 * user defined test units. The user derives own
 * test unit classes with several test methods.
 * These methods must be added to the test_unit class
 * with the contructor.
 * With a concrete test_unit serializable all or one test
 * method can be executed.
 */
class OOS_UNIT_API unit_test
{
public:
  /**
   * @brief A shortcut for the test function serializable.
   * 
   * A shortcut for the test function serializable.
   */
  typedef std::function<void ()> test_func;

  /**
   * @brief The constructor for a test_unit
   *
   * @param name The name of a test_unit serializable.
   * @param caption The caption of a test_unit serializable.
   */
  unit_test(std::string name, std::string caption);
  virtual ~unit_test();
  
  /**
   * @brief Initializes a test_unit test.
   *
   * Called before each executed test method. Can be
   * overwritten by the derived test_unit class.
   */
  virtual void initialize() {};

  /**
   * @brief Cleans up a test_unit test.
   *
   * Called after every executed test method. Can be
   * overwritten by the derived test_unit class.
   */
  virtual void finalize() {};

  /**
   * Returns the name of the test_unit.
   *
   * @return The name of the test_unit.
   */
  std::string name() const;

  /**
   * Returns the caption of the test_unit.
   *
   * @return The caption of the test_unit.
   */
  std::string caption() const;

  /**
   * @brief Executes each test method.
   *
   * Each test method of the test_unit is
   * executed.
   *
   * @param quiet If true no output is written to stdout
   */
  bool execute(bool quiet = false);

  /**
   * @brief Executes a concrete test method.
   * 
   * A single test method with the given name
   * is executed. If the test method couldn't
   * a warning is displayed.
   * 
   * @param test Name of the test to execute.
   * @param quiet If true no output is written to stdout
   */
  bool execute(const std::string &test, bool quiet = false);

  /**
   * @brief List all tests.
   * 
   * All tests and their descriptions are printed
   * to the given ostream.
   * 
   * @param out The stream to be written on.
   */
  void list(std::ostream &out) const;
  
  /**
   * @brief Adds a test to the test_unit.
   * 
   * This method adds a new test function identified
   * by the given name to the test_unit. Additionaly
   * a short description must be given.
   *
   * @param name unique name of the test to store.
   * @param test The test function serializable.
   * @param caption A short description of the test.
   */
  void add_test(const std::string &name, const test_func &test, const std::string &caption);

  /**
   * @brief Checks if a is equal b.
   *
   * If a is not equal b the test method throws
   * a unit_exception with the given message. The
   * exception is caught by the test_suite and the
   * message is displayed.
   * 
   * @tparam X The type of the left and right hand serializable to compare.
   * @param a The left hand operand.
   * @param b The right hand operand.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  template < class X >
  void assert_equal(const X &a, const X &b, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->assertion_count;
    if (a != b) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }

  /**
   * @brief Checks if a is equal b.
   *
   * If a is not equal b the test method throws
   * a unit_exception with the given message. The
   * exception is caught by the test_suite and the
   * message is displayed.
   *
   * @tparam X The type of the left hand serializable to compare.
   * @tparam Y The type of the right hand serializable to compare.
   * @param a The left hand operand.
   * @param b The right hand operand.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  template < class X, class Y >
  void assert_equal(const X &a, const Y &b, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->assertion_count;
    if (a != b) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }

#ifndef MATADOR_DOXYGEN_DOC
  void assert_equal(char *&a, const char* &b, const std::string &msg, int line, const char *file);
  void assert_equal(const std::string &a, const char *b, const std::string &msg, int line, const char *file);
  void assert_equal(const char *a, const std::string &b, const std::string &msg, int line, const char *file);
  void assert_equal(const char *a, const char *b, const std::string &msg, int line, const char *file);

  template < int N1, int N2 >
  void
  assert_equal(const char (&a)[N1], const char (&b)[N2], const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->assertion_count;
    if (strcmp(a, b) != 0) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }

  template < int N1, int N2 >
  void
  assert_equal(char (&a)[N1], const char (&b)[N2], const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->assertion_count;
    if (strcmp(a, b) != 0) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }

  template < class X >
  void assert_equal(const X &a, const bool &b, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->assertion_count;
    bool cmp = a > 0;
    if (cmp != b) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }

  void assert_equal(const bool &a, const bool &b, const std::string &msg, int line, const char *file);
#endif /* MATADOR_DOXYGEN_DOC */

  /**
   * @brief Checks if a is not equal b.
   *
   * If a is equal b the test method throws
   * a unit_exception with the given message. The
   * exception is caught by the test_suite and the
   * message is displayed
   * 
   * @tparam T The type of the objects to compare.
   * @param a The left hand operand.
   * @param b The right hand operand.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  template < class T >
  void assert_not_equal(const T &a, const T &b, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->assertion_count;
    if (a == b) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }

  /**
   * @brief Checks if a is not equal b.
   *
   * If a is equal b the test method throws
   * a unit_exception with the given message. The
   * exception is caught by the test_suite and the
   * message is displayed
   *
   * @tparam T The type of the left hand object to compare.
   * @tparam V The type of the right hand object to compare.
   * @param a The left hand operand.
   * @param b The right hand operand.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  template < class T, class V >
  void assert_not_equal(const T &a, const V &b, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->assertion_count;
    if (a == b) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }

  /**
   * @brief Checks if a is greater b.
   *
   * If a is not greater b the test method throws
   * a unit_exception with the given message. The
   * exception is caught by the test_suite and the
   * message is displayed.
   * 
   * @tparam T The type of the objects to compare.
   * @param a The left hand operand.
   * @param b The right hand operand.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  template < class T >
  void assert_greater(const T &a, const T &b, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->assertion_count;
    if (a <= b) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not greater " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }

#ifndef MATADOR_DOXYGEN_DOC
  template < class X, class Y >
  void assert_greater(const X &a, const Y &b, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->assertion_count;
    if (a <= b) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not greater " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }
#endif /* MATADOR_DOXYGEN_DOC */

  /**
   * @brief Checks if a is less b.
   *
   * If a is greater equal b the test method throws
   * a unit_exception with the given message. The
   * exception is caught by the test_suite and the
   * message is displayed.
   * 
   * @tparam T The type of the objects to compare.
   * @param a The left hand operand.
   * @param b The right hand operand.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  template < class T >
  void assert_less(const T &a, const T &b, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->assertion_count;
    if (a >= b) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is greater equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }

  /**
   * @brief Checks if the pointer to a is NULL.
   *
   * If the pointer to isn't NULL the test method
   * throws a unit_exception with the given message.
   * The exception is caught by the test_suite the
   * and message is displayed
   * 
   * @tparam T The type of the objects to compare.
   * @param a The value to evaluate.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  template < class T >
  void assert_null(const T *a, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->assertion_count;
    if (a != nullptr) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not null: " << msg;
      throw unit_exception(msgstr.str());
    }
  }

  /**
   * @brief Checks if the pointer to a is not NULL.
   *
   * If the pointer to is NULL the test method
   * throws a unit_exception with the given message.
   * The exception is caught by the test_suite the
   * and message is displayed
   * 
   * @tparam T The type of the objects to compare.
   * @param a The value to evaluate.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  template < class T >
  void assert_not_null(const T *a, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->assertion_count;
    if (a == 0) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is null: " << msg;
      throw unit_exception(msgstr.str());
    }
  }

  /**
   * @brief Checks if a evaluates to true.
   *
   * If a doesn't evaluates to true the test method
   * throws a unit_exception with the given message.
   * The exception is caught by the test_suite and the
   * message is displayed
   * 
   * @tparam T The type of the objects to compare.
   * @param a The value to evaluate.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  void assert_true(bool a, const std::string &msg, int line, const char *file);

  /**
   * @brief Checks if a evaluates to false.
   *
   * If a doesn't evaluates to false the test method
   * throws a unit_exception with the given message.
   * The exception is caught by the test_suite and the
   * message is displayed
   * 
   * @tparam T The type of the objects to compare.
   * @param a The value to evaluate.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  void assert_false(bool a, const std::string &msg, int line, const char *file);

  /**
   * @brief Checks if a evaluates to true.
   *
   * If a doesn't evaluates to true the test method
   * prints the given message.
   *
   * @tparam T The type of the objects to compare.
   * @param a The value to evaluate.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  void expect_true(bool a, const std::string &msg, int line, const char *file);

  /**
   * @brief Checks if a evaluates to false.
   *
   * If a doesn't evaluates to false the test method
   * prints the given message.
   * The exception is caught by the test_suite and the
   * message is displayed
   *
   * @tparam T The type of the objects to compare.
   * @param a The value to evaluate.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  void expect_false(bool a, const std::string &msg, int line, const char *file);

  /**
   * @brief Checks if a is equal b.
   *
   * If a is not equal b the test method prints
   * the given message to stdout.
   *
   * @tparam X The type of the left and right hand serializable to compare.
   * @param a The left hand operand.
   * @param b The right hand operand.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  template < class X >
  void expect_equal(const X &a, const X &b, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->error_count;
    if (a != b) {
      ++current_test_func_info->errors;
      std::cout << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
    }
  }

  /**
   * @brief Checks if a is equal b.
   *
   * If a is not equal b the test method prints
   * the given message to stdout.
   *
   * @tparam X The type of the left hand serializable to compare.
   * @tparam Y The type of the right hand serializable to compare.
   * @param a The left hand operand.
   * @param b The right hand operand.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  template < class X, class Y >
  void expect_equal(const X &a, const Y &b, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->error_count;
    if (a != b) {
      ++current_test_func_info->errors;
      std::cout << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
    }
  }
#ifndef MATADOR_DOXYGEN_DOC
  void expect_equal(const double &a, const double &b, const std::string &msg, int line, const char *file);
  void expect_equal(const char *a, const std::string &b, const std::string &msg, int line, const char *file);
  void expect_equal(const std::string &a, const char *b, const std::string &msg, int line, const char *file);
  void expect_equal(const char *a, const char *b, const std::string &msg, int line, const char *file);
  template < int N1, int N2 >
  void
  expect_equal(const char (&a)[N1], const char (&b)[N2], const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->error_count;
    if (strcmp(a, b) != 0) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
    }
  }
  template < int N1, int N2 >
  void
  expect_equal(char (&a)[N1], const char (&b)[N2], const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->error_count;
    if (strcmp(a, b) != 0) {
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
    }
  }
#endif
  /**
   * @brief Checks if a is greater b.
   *
   * If a is not greater b the test method prints
   * the given message to stdout.
   *
   * @tparam T The type of the objects to compare.
   * @param a The left hand operand.
   * @param b The right hand operand.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  template < class T >
  void expect_greater(const T &a, const T &b, const std::string &msg, int line, const char *file)
  {
    ++current_test_func_info->error_count;
    if (a <= b) {
      ++current_test_func_info->errors;
      std::cout << "FAILURE at " << file << ":" << line << ": value " << a << " is not greater " << b << ": " << msg;
    }
  }

  /**
   * @brief Throws an error.
   *
   * The test method throws an exception
   * with the given error message and the current
   * file and line number information.
   * 
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  void error(const std::string &msg, int line, const char *file);

  /**
   * @brief Displays a warning.
   *
   * The test method displays a warning
   * with the given message and the current
   * file and line number information.
   * 
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  void warn(const std::string &msg, int line, const char *file);

  /**
   * @brief Displays an information.
   *
   * The test method displays an information
   * with the given message.
   * 
   * @param msg The message to print if the check fails.
   */
  void info(const std::string &msg);

private:
  friend class test_suite;

  typedef struct test_func_info_struct
  {
    test_func_info_struct(const test_func &f, const std::string &n, const std::string &c)
      : func(f), name(n), caption(c)
    {}
    enum t_state {
      STATE_UNKNOWN = 0,
      STATE_SUCCEEDED,
      STATE_ERROR,
      STATE_FAILURE
    };

    bool is_error() const { return state == STATE_ERROR; }
    bool is_failure() const { return state == STATE_FAILURE; }
    bool is_succeeded() const { return state == STATE_SUCCEEDED; }
    test_func func;
    bool succeeded = true;
    t_state state = STATE_UNKNOWN;
    long duration;
    size_t assertion_count = 0;
    size_t error_count = 0;
    size_t errors = 0;
    std::string name;
    std::string caption;
    std::string message;
  } test_func_info;

private:
  void execute(test_func_info &test_info, bool quiet);

private:
  std::string name_;
  std::string caption_;

  typedef std::vector<test_func_info> t_test_func_info_vector;
  t_test_func_info_vector test_func_infos_;

  // runtime data
  test_func_info *current_test_func_info = nullptr;
};

}

#endif /* UNIT_TEST_HPP */
