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

#ifndef JSON_OBJECT_HPP
#define JSON_OBJECT_HPP

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
#include "json/json_string.hpp"

#include <string>
#include <map>

namespace oos {

/**
 * @class json_object
 * @brief Class representing the json object type.
 * 
 * This class represents the json object type.
 */
class OOS_API json_object : public json_type
{
public:
  typedef std::map<json_string, json_value> t_string_value_map; /**< Shortcut for the string value map */
  typedef t_string_value_map::const_iterator const_iterator;    /**< Shortcut for the map const iterator */
  typedef t_string_value_map::iterator iterator;                /**< Shortcut for the map iterator */
  typedef t_string_value_map::size_type size_type;              /**< Shortcut for the map size type */

public:
  json_object();
  /**
   * Creates a new json_object and initializes
   * the value with the given json_value.
   * 
   * @param x The json_value to set.
   */
  json_object(const json_value &x);

  /**
   * Assign from a json_value
   * 
   * @param x The json_value to assign from.
   * @return This.
   */
  json_object& operator=(const json_value &x);
  virtual ~json_object();

  virtual bool parse(std::istream &in);
  virtual void print(std::ostream &out) const;

  /**
   * Removes every children of the json_object.
   */
  void clear();
  
  /**
   * Returns true if the json_object
   * doesn't have a child element.
   * 
   * @return True if json_object is empty.
   */
  bool empty() const;

  /**
   * Return the begin iterator of
   * the children map.
   * 
   * @return The begin iterator.
   */
  iterator begin();

  /**
   * Return the constant begin iterator of
   * the children map.
   * 
   * @return The constant begin iterator.
   */
  const_iterator begin() const;

  /**
   * Return the end iterator of
   * the children map.
   * 
   * @return The end iterator.
   */
  iterator end();

  /**
   * Return the constant end iterator of
   * the children map.
   * 
   * @return The constant end iterator.
   */
  const_iterator end() const;

  /**
   * Returns true if the json_object contains
   * a child element with the given key.
   * 
   * @param key The key to validate.
   * @return True if the key exists.
   */
  bool contains(const std::string &key);
  
  /**
   * Inserts a json_value with a given key.
   * 
   * @param key The key of the json_value to insert.
   * @param value The json_value to insert.
   */
  void insert(const json_string &key, const json_value &value);

  /**
   * Returns a json_value with the given key.
   * 
   * @param key The key of the json_value.
   * @return the corresponding json_value.
   */
  virtual json_value& operator[](const std::string &key);

  /**
   * Returns the size of the json_object
   * 
   * @return The size of the json_object.
   */
  virtual size_t size() const { return string_value_map_.size(); }

  /**
   * Input operator for a json_object.
   * 
   * @param str The input stream to read from.
   * @param obj The json_object read in.
   * @return The modified input stream.
   */
  friend OOS_API std::istream& operator>>(std::istream &str, json_object &obj);

private:
  t_string_value_map string_value_map_;
};

}

#endif /* JSON_OBJECT_HPP */
