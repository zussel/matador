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
class object_vector_base;
class object_container;
class object_base_ptr;

/**
 * @class object_writer
 * @brief Base class for all object writer
 * 
 * This class is the base class for all
 * atomizer classes. It has interfaces to 
 * write data types (builtin and complex)
 * identified by a name.
 */
class OOS_API object_writer
{
public:
	virtual ~object_writer() {}

  /**
   * @fn virtual void write(const char *id, char x)
   * @brief Write a single character to the atomizer.
   * 
   * Write a single character to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write(const char*, char) {}

  /**
   * @fn virtual void write(const char *id, float x)
   * @brief Write a float to the atomizer.
   * 
   * Write a float to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write(const char*, float) {}

  /**
   * @fn virtual void write(const char *id, double x)
   * @brief Write a double to the atomizer.
   * 
   * Write a double to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write(const char*, double) {}

  /**
   * @fn virtual void write(const char *id, short x)
   * @brief Write a short to the atomizer.
   * 
   * Write a short to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
  virtual void write(const char*, short) {}

  /**
   * @fn virtual void write(const char *id, int x)
   * @brief Write a int to the atomizer.
   * 
   * Write a int to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write(const char*, int) {}

  /**
   * @fn virtual void write(const char *id, long x)
   * @brief Write a long to the atomizer.
   * 
   * Write a long to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write(const char*, long) {}

  /**
   * @fn virtual void write(const char *id, unsigned short x)
   * @brief Write a unsigned short to the atomizer.
   * 
   * Write a unsigned short to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
  virtual void write(const char*, unsigned short) {}

  /**
   * @fn virtual void write(const char *id, unsigned int x)
   * @brief Write a unsigned to the atomizer.
   * 
   * Write a unsigned int to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
  virtual void write(const char*, unsigned int) {}

  /**
   * @fn virtual void write(const char *id, unsigned long x)
   * @brief Write a unsigned long to the atomizer.
   * 
   * Write a unsigned long to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
  virtual void write(const char*, unsigned long) {}

  /**
   * @fn virtual void write(const char *id, bool x)
   * @brief Write a bool to the atomizer.
   * 
   * Write a bool to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write(const char*, bool) {}

  /**
   * @fn virtual void write(const char *id, const char *x, int max_size)
   * @brief Write a const char pointer to the atomizer.
   * 
   * Write a const char pointer to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   * @param max_size Maximum size of the character array
   */
	virtual void write(const char*, const char*, int) {}

  /**
   * @fn virtual void write(const char *id, const std::string &x)
   * @brief Write a std::string to the atomizer.
   * 
   * Write a std::string to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write(const char*, const std::string&) {}

  /**
   * @fn virtual void write(const char *id, const varchar_base &x)
   * @brief Write a std::string to the atomizer.
   * 
   * Write a varchar to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write(const char*, const varchar_base&) {}

  /**
   * @fn virtual void write(const char *id, const object_base_ptr &x)
   * @brief Write a object_base_ptr to the atomizer.
   * 
   * Write a object_base_ptr to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write(const char*, const object_base_ptr&) {}

  /**
   * @fn virtual void write(const char *id, const object_container &x)
   * @brief Write a object_container to the atomizer.
   * 
   * Write a object_container to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
  virtual void write(const char*, const object_container&) {}
};

class OOS_API object_reader
{
public:
  virtual ~object_reader() {}
  /**
   * @fn virtual void read(const char *id, char &x)
   * @brief Read a single character from the atomizer.
   * 
   * Read a single character from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char*, char&) {}

  /**
   * @fn virtual void read(const char *id, float &x)
   * @brief Read a float from the atomizer.
   * 
   * Read a float from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char*, float&) {}

  /**
   * @fn virtual void read(const char *id, double &x)
   * @brief Read a double from the atomizer.
   * 
   * Read a double from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char*, double&) {}

  /**
   * @fn virtual void read(const char *id, short &x)
   * @brief Read an short from the atomizer.
   * 
   * Read an short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char*, short&) {}

  /**
   * @fn virtual void read(const char *id, int &x)
   * @brief Read an integer from the atomizer.
   * 
   * Read an integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char*, int&) {}

  /**
   * @fn virtual void read(const char *id, long &x)
   * @brief Read a long from the atomizer.
   * 
   * Read a long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char*, long&) {}

  /**
   * @fn virtual void read(const char *id, unsigned short &x)
   * @brief Read an unsigned short from the atomizer.
   * 
   * Read a unsigned short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char*, unsigned short&) {}

  /**
   * @fn virtual void read(const char *id, unsigned int &x)
   * @brief Read an unsigned integer from the atomizer.
   * 
   * Read a unsigned integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char*, unsigned int&) {}

  /**
   * @fn virtual void read(const char *id, unsigned long &x)
   * @brief Read an unsigned long from the atomizer.
   * 
   * Read a unsigned long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char*, unsigned long&) {}

  /**
   * @fn virtual void read(const char *id, bool &x)
   * @brief Read a bool from the atomizer.
   * 
   * Read a bool from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char*, bool&) {}

  /**
   * @fn virtual void read(const char *id, char *x, int max_size)
   * @brief Read a const char pointer from the atomizer.
   * 
   * Read a const char pointer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   * @param max_size Maximum size of the character array
   */
	virtual void read(const char*, char*, int) {}

  /**
   * @fn virtual void read(const char *id, std::string &x)
   * @brief Read a std::string from the atomizer.
   * 
   * Read a std::string from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char*, std::string&) {}

  /**
   * @fn virtual void read(const char *id, varchar_base &x)
   * @brief Read a varchar from the atomizer.
   * 
   * Read a varchar from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char*, varchar_base&) {}

  /**
   * @fn virtual void read(const char *id, object_base_ptr &x)
   * @brief Read an object_base_ptr from the atomizer.
   * 
   * Read an object_base_ptr from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char*, object_base_ptr&) {}

  /**
   * @fn virtual void read(const char *id, object_container &x)
   * @brief Read an object_container from the atomizer.
   * 
   * Read an object_container from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char*, object_container&) {}
};

}

#endif /* OBJECTATOMIZER_HPP */
