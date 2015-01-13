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

#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

#ifdef _MSC_VER
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

#include <istream>
#include <memory>

namespace oos {

class json_object;
class json_bool;
class json_string;
class json_array;
class json_number;
class json_null;
class json_type;

/**
 * @class json_value
 * @brief Wrapper class for a concrete value
 * 
 * This class is a wrapper class for a concrete
 * value. It holds an object of json_type which
 * implements the concrete type.
 */
class OOS_API json_value
{
public:
  /**
   * Default constructor
   */
  json_value();
  /**
   * Copy constructor
   * 
   * @param x Value to copy from.
   */
  json_value(const json_value &x);

  /**
   * Creates a new json_value and
   * initializes it with a concrete
   * json_type.
   * 
   * @param x A concrete json_type object.
   */
  json_value(json_type *x);

  /**
   * Creates a new json_value and initializes
   * it with a string. Internaly an object of
   * type json_string is created.
   * 
   * @param x The string to initialize from.
   */
  json_value(const std::string &x);

  /**
   * Creates a new json_value and initializes
   * it with a string. Internaly an object of
   * type json_string is created.
   * 
   * @param x The string to initialize from.
   */
  json_value(const char *x);

  /**
   * Creates a new json_value and initializes
   * it with a integer. Internaly an object of
   * type json_number is created.
   * 
   * @param x The integer to initialize from.
   */
  json_value(int x);

  /**
   * Creates a new json_value and initializes
   * it with a double. Internaly an object of
   * type json_number is created.
   * 
   * @param x The double to initialize from.
   */
  json_value(double x);

  /**
   * Creates a new json_value and initializes
   * it with a boolean. Internaly an object of
   * type json_bool is created.
   * 
   * @param x The boolean to initialize from.
   */
  json_value(bool x);

  /**
   * Assign from a json_value
   *
   * @param x The json_value to assign from.
   * @return The initialized json_value.
   */
  json_value& operator=(const json_value &x);

  /**
   * Assign from a json_type
   *
   * @param x The json_type to assign from.
   * @return The initialized json_value.
   */
  json_value& operator=(json_type *x);

  /**
   * Assign from a std::string
   *
   * @param x The std::string to assign from.
   * @return The initialized json_value.
   */
  json_value& operator=(const std::string &x);

  /**
   * Assign from a character array
   *
   * @param x The character array to assign from.
   * @return The initialized json_value.
   */
  json_value& operator=(const char *x);

  /**
   * Assign from a double value
   *
   * @param x The double value to assign from.
   * @return The initialized json_value.
   */
  json_value& operator=(double x);

  /**
   * Assign from a boolean value
   *
   * @param x The boolean value to assign from.
   * @return The initialized json_value.
   */
  json_value& operator=(bool x);
  ~json_value();

  /**
   * Returns a json_value with the given key.
   * If the concrete type doesn't support this
   * method an exception is thrown.
   * 
   * @param key The key of the json_value.
   * @return the corresponding json_value.
   */
  json_value& operator[](const std::string &key);

  /**
   * Returns a json_value at the given index.
   * If the concrete type doesn't support this
   * method an exception is thrown.
   * 
   * @param index The index of the json_value.
   * @return the corresponding json_value.
   */
  json_value& operator[](size_t index);

  /**
   * Returns a json_value at the given index.
   * If the concrete type doesn't support this
   * method an exception is thrown.
   * 
   * @param index The index of the json_value.
   * @return the corresponding json_value.
   */
  const json_value& operator[](size_t index) const;

  /**
   * Adds a json_value to the current value.
   * If the concrete type doesn't support this
   * method an exception is thrown.
   * 
   * @param x The json_value to add.
   */
  void push_back(const json_value &x);

  /**
   * Returns the size of the json_value.
   * If the concrete type doesn't support this
   * method an exception is thrown.
   * 
   * @return The size of the json_value.
   */
  size_t size() const;

  /**
   * Creates a json_value from the
   * given input stream.
   * 
   * @param in The input stream to read from.
   * @return The newly created json_value.
   */
  static json_value create(std::istream &in);

  /**
   * Input operator for a json_value.
   * 
   * @param str The input stream to read from.
   * @param value The json_value read in.
   * @return The modified input stream.
   */
  friend OOS_API std::istream& operator>>(std::istream &str, json_value &value);

  /**
   * Output operator for json_value. Each
   * concrete type overloads a print method,
   * to have it print out correctly.
   * 
   * @param str The output stream to print on.
   * @param value The json_value to print.
   * @return The modified output stream.
   */
  friend OOS_API std::ostream& operator<<(std::ostream &str, const json_value &value);

  /**
   * Return true if the json_value
   * is of concrete type T.
   * 
   * @tparam T The requested type info
   * @return True if the type matches
   */
  template < class T >
  bool is_type() const
  {
    return dynamic_cast<const T*>(type_.get()) != 0;
  }

  /**
   * Returns the concrete value. If the
   * value couldn't be cast to the
   * requested type null is returned.
   * 
   * @tparam The requested value type.
   * @return The concrete value of json_value.
   */
  template < class T >
  const T* value_type() const
  {
    return dynamic_cast<const T*>(type_.get());
  }

//  template < class T >
private:
  std::tr1::shared_ptr<json_type> type_;
};

}

#endif /* JSON_VALUE_HPP */
