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

#ifndef STATEMENT_SERIALIZER_HPP
#define STATEMENT_SERIALIZER_HPP

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

namespace oos {

class statement_impl;
class object;

/// @cond OOS_DEV

class OOS_API statement_serializer : public object_atomizer
{
public:
  statement_serializer();
  virtual ~statement_serializer();

  void bind(statement_impl *stmt, object *o, bool bind_id);
  void read(statement_impl *stmt, object *o);

private:
  /**
   * @brief Read a single character from the atomizer.
   * 
   * Read a single character from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char *id, char &x);

  /**
   * @brief Read a float from the atomizer.
   * 
   * Read a float from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char *id, float &x);

  /**
   * @brief Read a double from the atomizer.
   * 
   * Read a double from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char *id, double &x);

  /**
   * @brief Read a short from the atomizer.
   * 
   * Read a short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, short &x);

  /**
   * @brief Read an integer from the atomizer.
   * 
   * Read an integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, int &x);

  /**
   * @brief Read a long from the atomizer.
   * 
   * Read a long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, long &x);

  /**
   * @brief Read an unsigned short from the atomizer.
   * 
   * Read a unsigned short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, unsigned short &x);

  /**
   * @brief Read an unsigned integer from the atomizer.
   * 
   * Read a unsigned integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, unsigned int &x);

  /**
   * @brief Read an unsigned long from the atomizer.
   * 
   * Read a unsigned long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, unsigned long &x);

  /**
   * @brief Read a bool from the atomizer.
   * 
   * Read a bool from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, bool &x);

  /**
   * @brief Read a const char pointer from the atomizer.
   * 
   * Read a const char pointer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, char* &x);

  /**
   * @brief Read a std::string from the atomizer.
   * 
   * Read a std::string from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, std::string &x);

  /**
   * @brief Read a varchar from the atomizer.
   * 
   * Read a varchar from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char *id, varchar_base &x);
    
  /**
   * @brief Read an object_base_ptr from the atomizer.
   * 
   * Read an object_base_ptr from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, object_base_ptr &x);

  /**
   * @brief Write a single character to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, char x);

  /**
   * @brief Write a float to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, float x);

  /**
   * @brief Write a double to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, double x);

  /**
   * @brief Write a short to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, short x);

  /**
   * @brief Write a int to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, int x);

  /**
   * @brief Write a long to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, long x);

  /**
   * @brief Write an unsigned short to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, unsigned short x);

  /**
   * @brief Write an unsigned int to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, unsigned int x);

  /**
   * @brief Write an unsigned long to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, unsigned long x);

  /**
   * @brief Write a bool to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, bool x);

  /**
   * @brief Write a const character string to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, const char *x);

  /**
   * @brief Write a std::string to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, const std::string &x);

  /**
   * @brief Write a varchar_base to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void write(const char *id, const varchar_base &x);

  /**
   * @brief Write an object_base_ptr to the binder.
   * 
   * Add the value of the column identified by the
   * to the sql statement. In this case the id of the
   * foreign object is written to the column.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, const object_base_ptr &x);

private:
  bool valid_column(const char *id, int i) const;

private:
  statement_impl *stmt_;
  
  int column_;
};

/// @endcond

}

#endif /* STATEMENT_SERIALIZER_HPP */