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

#ifndef STATEMENT_HELPER_HPP
#define STATEMENT_HELPER_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
#else
  #define OOS_API
#endif

#include "object/object_atomizer.hpp"

#include <sstream>

namespace oos {

class object;

/// @cond OOS_DEV
class OOS_API statement_helper : public object_atomizer
{
public:
  statement_helper();
  virtual ~statement_helper();

  typedef enum {
    UNKNOWN = 0,
    SELECT,
    CREATE,
    INSERT,
    UPDATE,
    DEL,
    DROP
  } t_type;

  std::string create(object *o, const std::string &table_name, t_type type, const std::string &where = "");

private:
  /**
   * @fn virtual void write_char(const char *id, char x)
   * @brief Write a single character to the atomizer.
   * 
   * Write a single character to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_char(const char *id, char x);

  /**
   * @fn virtual void write_float(const char *id, float x)
   * @brief Write a float to the atomizer.
   * 
   * Write a float to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_float(const char *id, float x);

  /**
   * @fn virtual void write_double(const char *id, double x)
   * @brief Write a double to the atomizer.
   * 
   * Write a double to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_double(const char *id, double x);

  /**
   * @fn virtual void write_int(const char *id, int x)
   * @brief Write a int to the atomizer.
   * 
   * Write a int to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_int(const char *id, int x);

  /**
   * @fn virtual void write_long(const char *id, long x)
   * @brief Write a long to the atomizer.
   * 
   * Write a long to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_long(const char *id, long x);

  /**
   * @fn virtual void write_unsigned(const char *id, unsigned x)
   * @brief Write a unsigned to the atomizer.
   * 
   * Write a unsigned to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_unsigned(const char *id, unsigned x);

  /**
   * @fn virtual void write_bool(const char *id, bool x)
   * @brief Write a bool to the atomizer.
   * 
   * Write a bool to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_bool(const char *id, bool x);

  /**
   * @fn virtual void write_charptr(const char *id, const char *x)
   * @brief Write a const char pointer to the atomizer.
   * 
   * Write a const char pointer to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_charptr(const char *id, const char *x);

  /**
   * @fn virtual void write_string(const char *id, const std::string &x)
   * @brief Write a std::string to the atomizer.
   * 
   * Write a std::string to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_string(const char *id, const std::string &x);

  /**
   * @fn virtual void write_varchar(const char *id, const varchar_base &x)
   * @brief Write a std::string to the atomizer.
   * 
   * Write a varchar to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_varchar(const char *id, const varchar_base &x);

  /**
   * @fn virtual void write_object(const char *id, const object_base_ptr &x)
   * @brief Write a object_base_ptr to the atomizer.
   * 
   * Write a object_base_ptr to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_object(const char *id, const object_base_ptr &x);

private:
  t_type type_;
  object *object_;
  std::stringstream statement_;
  int param_count_;
};
/// @endcond
}

#endif /* STATEMENT_HELPER_HPP */
