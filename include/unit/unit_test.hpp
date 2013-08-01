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

#ifndef OOS_DOXYGEN_DOC
  #ifdef WIN32
    #ifdef oos_EXPORTS
      #define OOS_API __declspec(dllexport)
      #define EXPIMP_TEMPLATE
    #else
      #define OOS_API __declspec(dllimport)
      #define EXPIMP_TEMPLATE extern
    #endif
    #pragma warning(disable: 4251)
  #else
    #define OOS_API
  #endif
  #ifdef WIN32
    #define CPP11_TYPE_TRAITS_NS std::tr1
  #else
  #define CPP11_TYPE_TRAITS_NS std
#endif

#endif /* OOS_DOXYGEN_DOC */

#include "unit/unit_exception.hpp"

#include "tools/enable_if.hpp"

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

#include <map>

#include <cstring>
#include <string>
#include <sstream>
#include <type_traits>

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
 * @brief Checks if a is not equal b.
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
 * @brief Throws an error.
 *
 * The test method throws an exception
 * with the given error message and the current
 * file and line number information.
 */
#define UNIT_FAIL(msg)                   error(msg, __LINE__, __FILE__)

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
    warn_stream << msg;                  \
    warn(warn_stream.str(), __LINE__, __FILE__); \
  } while (false)

/**
 * @brief Displays an informatkon.
 *
 * The test method displays an information
 * with the given message.
 */
#define UNIT_INFO(msg)                   info(msg)

namespace oos {

class varchar_base;

/**
 * @class unit_test
 * @brief A unit_test consists of serveral tests.
 *
 * The unit_test class is the base class for all
 * user defined test units. The user derives own
 * test unit classes with several test methods.
 * These methods must be added to the test_unit class
 * with the contructor.
 * With a concrete test_unit object all or one test
 * method can be executed.
 */
class OOS_API unit_test
{
public:
  /**
   * @brief A shortcut for the test function object.
   * 
   * A shortcut for the test function object.
   */
  typedef std::tr1::function<void ()> test_func;

  /**
   * @brief The constructor for a test_unit
   *
   * @param name The name of a test_unit object.
   * @param caption The caption of a test_unit object.
   */
  unit_test(const std::string &name, const std::string &caption);
  virtual ~unit_test();
  
  /**
   * @brief Initializes a test_unit test.
   *
   * Called before each executed test method. Must be
   * overwritten by the derived test_unit class.
   */
  virtual void initialize() = 0;

  /**
   * @brief Cleans up a test_unit test.
   *
   * Called after every executed test method. Must be
   * overwritten by the derived test_unit class.
   */
  virtual void finalize() = 0;

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
   */
  void execute();

  /**
   * @brief Executes a concrete test method.
   * 
   * A single test method with the given name
   * is executed. If the test method couldn't
   * a warning is displayed.
   * 
   * @param test Name of the test to execute.
   */
  void execute(const std::string &test);

  /**
   * @brief List all tests.
   * 
   * All tests and their descriptions are printed
   * to the given ostream.
   * 
   * @param out The stream to be written on.
   */
  void list(std::ostream &out);
  
  /**
   * @brief Adds a test to the test_unit.
   * 
   * This method adds a new test function identified
   * by the given name to the test_unit. Additionaly
   * a short description must be given.
   *
   * @param name unique name of the test to store.
   * @param test The test function object.
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
   * @tparam X The type of the left hand object to compare.
   * @tparam Y The type of the right hand object to compare.
   * @param a The left hand operand.
   * @param b The right hand operand.
   * @param msg The message to print if the check fails.
   * @param line The line number of this check in the source code.
   * @param file The file where this check can be found.
   */
  template < class X, class Y >
  void assert_equal(const X &a, const Y &b, const std::string &msg, int line, const char *file)
  {
    if (a != b) {
      // throw exception
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }
#ifndef OOS_DOXYGEN_DOC
  template < int N1, int N2 >
  void
  assert_equal(const char (&a)[N1], const char (&b)[N2], const std::string &msg, int line, const char *file)
  {
    if (strcmp(a, b) != 0) {
      // throw exception
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }
  template < class X >
  void assert_equal(const X &a, const bool &b, const std::string &msg, int line, const char *file)
  {
    bool cmp = a > 0;
    if (cmp != b) {
      // throw exception
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }

  void assert_equal(const bool &a, const bool &b, const std::string &msg, int line, const char *file)
  {
    if (a != b) {
      // throw exception
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not equal " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }
#endif /* OOS_DOXYGEN_DOC */

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
    if (a == b) {
      // throw exception
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
    if (a <= b) {
      // throw exception
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not greater " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }

#ifndef OOS_DOXYGEN_DOC
  template < class X, class Y >
  void assert_greater(const X &a, const Y &b, const std::string &msg, int line, const char *file)
  {
    if (a <= b) {
      // throw exception
      std::stringstream msgstr;
      msgstr << "FAILURE at " << file << ":" << line << ": value " << a << " is not greater " << b << ": " << msg;
      throw unit_exception(msgstr.str());
    }
  }
#endif /* OOS_DOXYGEN_DOC */

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
    if (a >= b) {
      // throw exception
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
    if (a != 0) {
      // throw exception
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
    if (a == 0) {
      // throw exception
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
   * @brief Displays an informatkon.
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
    test_func_info_struct(const test_func &f, const std::string &c)
      : func(f), succeeded(true), caption(c)
    {}
    test_func func;
    bool succeeded;
    std::string caption;
    std::string message;
  } test_func_info;

private:
  void execute(test_func_info &test_info);

private:
  std::string name_;
  std::string caption_;

  typedef std::map<std::string, test_func_info> t_test_func_info_map;
  t_test_func_info_map test_func_info_map_;
};

}

#endif /* UNIT_TEST_HPP */
