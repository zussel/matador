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

#ifndef OBJECTATOMIZER_HPP
#define OBJECTATOMIZER_HPP

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

#include <string>

namespace oos {
class varchar_base;
class object_list_base;
class object_vector_base;
class object_base_ptr;

/**
 * @class object_atomizer
 * @brief Base class for all object atomizer
 * 
 * This class is the base class for all
 * atomizer classes. It has interfaces to read
 * and write data types (builtin and complex)
 * identified by a name.
 */
class OOS_API object_atomizer
{
public:
	virtual ~object_atomizer() {}

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
	virtual void write_char(const char*, char) {}

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
	virtual void write_float(const char*, float) {}

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
	virtual void write_double(const char*, double) {}

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
	virtual void write_int(const char*, int) {}

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
	virtual void write_long(const char*, long) {}

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
	virtual void write_unsigned(const char*, unsigned) {}

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
	virtual void write_bool(const char*, bool) {}

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
	virtual void write_charptr(const char*, const char*) {}

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
	virtual void write_string(const char*, const std::string&) {}

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
	virtual void write_varchar(const char*, const varchar_base&) {}

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
	virtual void write_object(const char*, const object_base_ptr&) {}

  /**
   * @fn virtual void write_object_list(const char *id, const object_list_base &x)
   * @brief Write a object_list_base to the atomizer.
   * 
   * Write a object_list_base to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_object_list(const char*, const object_list_base&) {}

  /**
   * @fn virtual void write_object_vector(const char *id, const object_vector_base &x)
   * @brief Write a object_list_base to the atomizer.
   * 
   * Write a object_vector_base to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write_object_vector(const char*, const object_vector_base&) {}

  /**
   * @fn virtual void read_char(const char *id, char &x)
   * @brief Read a single character from the atomizer.
   * 
   * Read a single character from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read_char(const char*, char&) {}

  /**
   * @fn virtual void read_float(const char *id, float &x)
   * @brief Read a float from the atomizer.
   * 
   * Read a float from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read_float(const char*, float&) {}

  /**
   * @fn virtual void read_double(const char *id, double &x)
   * @brief Read a double from the atomizer.
   * 
   * Read a double from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read_double(const char*, double&) {}

  /**
   * @fn virtual void read_int(const char *id, int &x)
   * @brief Read an integer from the atomizer.
   * 
   * Read an integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read_int(const char*, int&) {}

  /**
   * @fn virtual void read_long(const char *id, long &x)
   * @brief Read a long from the atomizer.
   * 
   * Read a long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read_long(const char*, long&) {}

  /**
   * @fn virtual void read_unsigned(const char *id, unsigned &x)
   * @brief Read an unsigned integer from the atomizer.
   * 
   * Read a unsigned integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read_unsigned(const char*, unsigned&) {}

  /**
   * @fn virtual void read_bool(const char *id, bool &x)
   * @brief Read a bool from the atomizer.
   * 
   * Read a bool from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read_bool(const char*, bool&) {}

  /**
   * @fn virtual void read_charptr(const char *id, char *&x)
   * @brief Read a const char pointer from the atomizer.
   * 
   * Read a const char pointer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read_charptr(const char*, char*&) {}

  /**
   * @fn virtual void read_string(const char *id, std::string &x)
   * @brief Read a std::string from the atomizer.
   * 
   * Read a std::string from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read_string(const char*, std::string&) {}

  /**
   * @fn virtual void read_varchar(const char *id, varchar &x)
   * @brief Read a varchar from the atomizer.
   * 
   * Read a varchar from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read_varchar(const char*, varchar_base&) {}

  /**
   * @fn virtual void read_object(const char *id, object_base_ptr &x)
   * @brief Read an object_base_ptr from the atomizer.
   * 
   * Read an object_base_ptr from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read_object(const char*, object_base_ptr&) {}

  /**
   * @fn virtual void read_object_list(const char *id, object_list_base &x)
   * @brief Read an object_list_base from the atomizer.
   * 
   * Read an object_list_base from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read_object_list(const char*, object_list_base&) {}

  /**
   * @fn virtual void read_object_vector(const char *id, object_vector_base &x)
   * @brief Read an object_vector_base from the atomizer.
   * 
   * Read an object_vector_base from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read_object_vector(const char*, object_vector_base&) {}
};

}

#endif /* OBJECTATOMIZER_HPP */
