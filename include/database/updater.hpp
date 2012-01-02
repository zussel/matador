/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY {} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INSERTER_HPP
#define INSERTER_HPP

#include "object/object_atomizer.hpp"

namespace oos {

class updater : public object_atomizer
{
public:
  updater();
  virtual ~updater();

  /**
   * @brief Write a single character to the updater.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write_char(const char *id, char x);

  /**
   * @brief Write a float to the updater.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write_float(const char *id, float x);

  /**
   * @brief Write a double to the updater.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write_double(const char *id, double x);

  /**
   * @brief Write a int to the updater.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write_int(const char *id, int x);

  /**
   * @brief Write a long to the updater.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write_long(const char *id, long x);

  /**
   * @brief Write an unsigned int to the updater.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write_unsigned(const char *id, unsigned x);

  /**
   * @brief Write a bool to the updater.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write_bool(const char *id, bool x);

  /**
   * @brief Write a const character string to the updater.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write_charptr(const char *id, const char *x);

  /**
   * @brief Write a std::string to the updater.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write_string(const char *id, const std::string &x);

  /**
   * @brief Write an object_base_ptr to the updater.
   * 
   * Add the value of the column identified by the
   * to the sql statement. In this case the id of the
   * foreign object is written to the column.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write_object(const char *id, const object_base_ptr &x);

  /**
   * @brief Write an object list to the updater.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write_object_list(const char *id, const object_list_base &x);

  /**
   * @brief Write a object vector to the updater.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write_object_vector(const char *id, const object_vector_base &x);
};

}

#endif /* INSERTER_HPP */
