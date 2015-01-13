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

#ifndef JSON_TYPE_HPP
#define JSON_TYPE_HPP

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

#include "json/json_parser.hpp"

#include <string>

namespace oos {

class json_value;

/**
 * @class json_type
 * @brief Base class for all json types
 * 
 * This is the base class for all json types
 * - array
 * - object
 * - string
 * - number
 * - boolean
 * - null
 * 
 * It provides all accessors to get the
 * values for each concrete class.
 */
class OOS_API json_type
{
protected:
  /**
   * Creates a json_type of a certain type
   * 
   * @param name Type of the json_type
   */
  explicit json_type(const std::string &name);

public:
  virtual ~json_type();

  /**
   * Parses in json input stream
   * 
   * @param in The json input stream
   * @return True if stream was parsed correctly
   */
  virtual bool parse(std::istream &in) = 0;

  /**
   * Prints the json type (tree) to
   * a output stream.
   * 
   * @param out The stream to write on.
   */
  virtual void print(std::ostream &out) const = 0;

  /**
   * Returns a json_value with the given key.
   * If the concrete type doesn't support this
   * method an exception is thrown.
   * 
   * @param key The key of the json_value.
   * @return the corresponding json_value.
   */
  virtual json_value& operator[](const std::string &key);

  /**
   * Returns a json_value at the given index.
   * If the concrete type doesn't support this
   * method an exception is thrown.
   * 
   * @param index The index of the json_value.
   * @return the corresponding json_value.
   */
  virtual json_value& operator[](size_t index);

  /**
   * Returns a json_value at the given index.
   * If the concrete type doesn't support this
   * method an exception is thrown.
   * 
   * @param index The index of the json_value.
   * @return the corresponding json_value.
   */
  virtual const json_value& operator[](size_t index) const;

  /**
   * Adds a json_value to the current value.
   * If the concrete type doesn't support this
   * method an exception is thrown.
   * 
   * @param x The json_value to add.
   */
  virtual void push_back(const json_value &x);

  /**
   * Returns the size of the json_value.
   * If the concrete type doesn't support this
   * method an exception is thrown.
   * 
   * @return The size of the json_value.
   */
  virtual size_t size() const;

  /**
   * Return the type string of
   * the json_type.
   * 
   * @return The type string.
   */
  std::string type() const;
  
  /**
   * Output operator for json_type. Each
   * concrete type overloads a print method,
   * to have it print out correctly.
   * 
   * @param str The output stream to print on.
   * @param value The json_type to print.
   * @return The modified output stream.
   */
  friend OOS_API std::ostream& operator<<(std::ostream &str, const json_type &value);

  /**
   * Input operator for json_object. In
   * order to parse an object the corresponding
   * input operator needs access to json_type.
   * 
   * @param str The input stream to print on.
   * @param obj The json_object to parse.
   * @return The modified input stream.
   */
  friend OOS_API std::istream& operator>>(std::istream &str, json_object &obj);

  /**
   * Input operator for json_array. In
   * order to parse an array the corresponding
   * input operator needs access to json_type.
   * 
   * @param str The input stream to print on.
   * @param obj The json_array to parse.
   * @return The modified input stream.
   */
  friend OOS_API std::istream& operator>>(std::istream &str, json_array &obj);

protected:
  /**
   * Sets the type string of
   * the json_type.
   * 
   * @param type The type string to set
   */
  void type(const std::string &type);

protected:
  static json_parser parser; /**< The json_parser object */

private:
  std::string type_;
};

}

#endif /* JSON_TYPE_HPP */
