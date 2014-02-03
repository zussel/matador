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

#ifndef JSON_ARRAY_HPP
#define JSON_ARRAY_HPP

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

#include "json/json_type.hpp"
#include "json/json_value.hpp"

#include <vector>
#include <stdexcept>

namespace oos {

/**
 * @class json_array
 * @brief Class representing the json array type.
 * 
 * This class represents the json array type.
 */
class OOS_API json_array : public json_type
{
public:
  typedef std::vector<json_value> t_value_vector;        /**< Shortcut for value vector. */
  typedef t_value_vector::const_iterator const_iterator; /**< Shortcut for value vector iterator. */
  typedef t_value_vector::iterator iterator;             /**< Shortcut for value vector const iterator. */
  typedef t_value_vector::size_type size_type;           /**< Shortcut for value vector size type. */

public:
  json_array();
  /**
   * Create a new json_array with a
   * predefined size.
   * 
   * @param size The size of the array.
   */
  explicit json_array(size_t size);
  
  /**
   * Creates a new json_array and initializes
   * the value with the given json_value.
   * 
   * @param x The json_value to set.
   */
  json_array(const json_value &x);

  /**
   * Assign from a json_value
   * 
   * @param x The json_value to assign from.
   * @return This.
   */
  json_array& operator=(const json_value &x);
  ~json_array();

  virtual bool parse(std::istream &in);
  virtual void print(std::ostream &out) const;

  /**
   * Return the begin iterator
   * 
   * @return The begin iterator.
   */
  iterator begin();
  /**
   * Return the constant begin iterator
   * 
   * @return The constant begin iterator.
   */
  const_iterator begin() const;

  /**
   * Return the end iterator
   * 
   * @return The end iterator.
   */
  iterator end();
  /**
   * Return the constant end iterator
   * 
   * @return The constant end iterator.
   */
  const_iterator end() const;

  /**
   * Returns true on empty array.
   * 
   * @return True on empty array.
   */
  bool empty() const;

  /**
   * Returns a json_value at the given index.
   * 
   * @param index The index of the json_value.
   * @return the corresponding json_value.
   */
  virtual json_value& operator[](size_t index);

  /**
   * Returns a json_value at the given index.
   * 
   * @param index The index of the json_value.
   * @return the corresponding json_value.
   */
  virtual const json_value& operator[](size_t index) const;

  /**
   * Adds a json_value to the current value.
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
   * Input operator for a json_array.
   * 
   * @param str The input stream to read from.
   * @param ary The json_array read in.
   * @return The modified input stream.
   */
  friend OOS_API std::istream& operator>>(std::istream &str, json_array &ary);

private:
  t_value_vector value_vector_;
};

}

#endif /* JSON_ARRAY_HPP */
