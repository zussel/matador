/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY = 0; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OBJECTATOMIZER_HPP
#define OBJECTATOMIZER_HPP

#ifdef _MSC_VER
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
class object_container;
class object_base_ptr;
class primary_key_base;
class date;
class time;

/**
 * @class object_writer
 * @brief Base class for all serializable writer
 * 
 * This class is the base class for all
 * atomizer classes. It has interfaces to 
 * write data types (builtin and complex)
 * identified by a name.
 */
class OOS_API object_writer
{
public:
  virtual ~object_writer() {};

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
	virtual void write(const char*, char) = 0;

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
	virtual void write(const char*, float) = 0;

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
	virtual void write(const char*, double) = 0;

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
  virtual void write(const char*, short) = 0;

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
	virtual void write(const char*, int) = 0;

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
	virtual void write(const char*, long) = 0;

  /**
   * @fn virtual void write(const char *id, unsigned char x)
   * @brief Write a unsigned char to the atomizer.
   * 
   * Write a unsigned char to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
  virtual void write(const char*, unsigned char) = 0;

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
  virtual void write(const char*, unsigned short) = 0;

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
  virtual void write(const char*, unsigned int) = 0;

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
  virtual void write(const char*, unsigned long) = 0;

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
	virtual void write(const char*, bool) = 0;

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
	virtual void write(const char*, const char*, int) = 0;

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
	virtual void write(const char*, const std::string&) = 0;

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
	virtual void write(const char*, const varchar_base&) = 0;

  /**
   * @fn virtual void write(const char *id, const date &x)
   * @brief Write a date to the atomizer.
   * 
   * Write a date to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write(const char*, const date&) = 0;

  /**
   * @fn virtual void write(const char *id, const time &x)
   * @brief Write a time to the atomizer.
   *
   * Write a time to the atomizer
   * identified by a unique name.
   *
   * @param id Unique id of the data.
   * @param x The data to read from.
   */
	virtual void write(const char*, const time&) = 0;

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
	virtual void write(const char*, const object_base_ptr&) = 0;

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
  virtual void write(const char*, const object_container&) = 0;
  
  virtual void write(const char*, const primary_key_base&) = 0;
};

/**
 * @tparam T Type of the generic serializable writer
 * @class generic_object_writer
 * @brief Generic base class for all serializable writer
 * 
 * This class implements the oos::object_writer interface
 * and adds the functionality to describe a write_value
 * method with a template.
 */
template < class T >
class generic_object_writer : public object_writer
{
protected:
  /**
   * @tparam T Type of the concrete writer
   * 
   * Constructs a generic_object_writer.
   */
  generic_object_writer(T *writer) : generic_writer_(writer) {}

public:
	virtual ~generic_object_writer() {}

/// @cond OOS_DEV

	virtual void write(const char *id, char x) { generic_writer_->write_value(id, x); }
	virtual void write(const char *id, float x) { generic_writer_->write_value(id, x); }
	virtual void write(const char *id, double x) { generic_writer_->write_value(id, x); }
  virtual void write(const char *id, short x) { generic_writer_->write_value(id, x); }
	virtual void write(const char *id, int x) { generic_writer_->write_value(id, x); }
	virtual void write(const char *id, long x) { generic_writer_->write_value(id, x); }
  virtual void write(const char *id, unsigned char x) { generic_writer_->write_value(id, x); }
  virtual void write(const char *id, unsigned short x) { generic_writer_->write_value(id, x); }
  virtual void write(const char *id, unsigned int x) { generic_writer_->write_value(id, x); }
  virtual void write(const char *id, unsigned long x) { generic_writer_->write_value(id, x); }
	virtual void write(const char *id, bool x) { generic_writer_->write_value(id, x); }
	virtual void write(const char *id, const char *x, int s) { generic_writer_->write_value(id, x, s); }
	virtual void write(const char *id, const std::string &x) { generic_writer_->write_value(id, x); }
	virtual void write(const char *id, const varchar_base &x) { generic_writer_->write_value(id, x); }
	virtual void write(const char *id, const date &x) { generic_writer_->write_value(id, x); }
	virtual void write(const char *id, const time &x) { generic_writer_->write_value(id, x); }
	virtual void write(const char *id, const object_base_ptr &x) { generic_writer_->write_value(id, x); }
  virtual void write(const char *id, const object_container &x) { generic_writer_->write_value(id, x); }
  virtual void write(const char *id, const primary_key_base &x) { generic_writer_->write_value(id, x); }

/// @endcond OOS_DEV
  
private:
  T *generic_writer_;
};

/**
 * @class object_reader
 * @brief Base class for all serializable reader
 * 
 * This class is the base class for all
 * atomizer reader classes. It has interfaces to 
 * read data types (builtin and complex)
 * identified by a name.
 */
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
  virtual void read(const char*, char&) = 0;

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
  virtual void read(const char*, float&) = 0;

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
  virtual void read(const char*, double&) = 0;

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
  virtual void read(const char*, short&) = 0;

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
	virtual void read(const char*, int&) = 0;

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
	virtual void read(const char*, long&) = 0;

  /**
   * @fn virtual void read(const char *id, unsigned char &x)
   * @brief Read an unsigned char from the atomizer.
   * 
   * Read a unsigned char from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
  virtual void read(const char*, unsigned char&) = 0;

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
  virtual void read(const char*, unsigned short&) = 0;

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
  virtual void read(const char*, unsigned int&) = 0;

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
  virtual void read(const char*, unsigned long&) = 0;

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
	virtual void read(const char*, bool&) = 0;

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
	virtual void read(const char*, char*, int) = 0;

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
	virtual void read(const char*, std::string&) = 0;

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
	virtual void read(const char*, varchar_base&) = 0;

  /**
  * @fn virtual void read(const char *id, date &x)
  * @brief Read a date from the atomizer.
  *
  * Read a time from the atomizer
  * identified by a unique name.
  *
  * @param id Unique id of the data.
  * @param x The data to write to.
  */
  virtual void read(const char*, date&) = 0;

  /**
  * @fn virtual void read(const char *id, time &x)
  * @brief Read a time from the atomizer.
  *
  * Read a time from the atomizer
  * identified by a unique name.
  *
  * @param id Unique id of the data.
  * @param x The data to write to.
  */
  virtual void read(const char*, time&) = 0;

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
	virtual void read(const char*, object_base_ptr&) = 0;

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
  virtual void read(const char*, object_container&) = 0;

  virtual void read(const char*, primary_key_base&) = 0;
};

/**
 * @tparam T Type of the generic serializable reader
 * @class generic_object_reader
 * @brief Generic base class for all serializable readre
 * 
 * This class implements the oos::object_reader interface
 * and adds the functionality to describe a read_value
 * method with a template.
 */
template < class T >
class generic_object_reader : public object_reader
{
protected:
  /**
   * @tparam T Type of the concrete reader
   * 
   * Constructs a generic_object_reader.
   */
  generic_object_reader(T *reader) : generic_reader_(reader) {}

public:
	virtual ~generic_object_reader() {}

/// @cond OOS_DEV

	virtual void read(const char *id, char &x) { generic_reader_->read_value(id, x); }
	virtual void read(const char *id, float &x) { generic_reader_->read_value(id, x); }
	virtual void read(const char *id, double &x) { generic_reader_->read_value(id, x); }
  virtual void read(const char *id, short &x) { generic_reader_->read_value(id, x); }
	virtual void read(const char *id, int &x) { generic_reader_->read_value(id, x); }
	virtual void read(const char *id, long &x) { generic_reader_->read_value(id, x); }
  virtual void read(const char *id, unsigned char &x) { generic_reader_->read_value(id, x); }
  virtual void read(const char *id, unsigned short &x) { generic_reader_->read_value(id, x); }
  virtual void read(const char *id, unsigned int &x) { generic_reader_->read_value(id, x); }
  virtual void read(const char *id, unsigned long &x) { generic_reader_->read_value(id, x); }
	virtual void read(const char *id, bool &x) { generic_reader_->read_value(id, x); }
	virtual void read(const char *id, char *x, int s) { generic_reader_->read_value(id, x, s); }
	virtual void read(const char *id, std::string &x) { generic_reader_->read_value(id, x); }
	virtual void read(const char *id, varchar_base &x) { generic_reader_->read_value(id, x); }
	virtual void read(const char *id, object_base_ptr &x) { generic_reader_->read_value(id, x); }
	virtual void read(const char *id, date &x) { generic_reader_->read_value(id, x); }
	virtual void read(const char *id, time &x) { generic_reader_->read_value(id, x); }
  virtual void read(const char *id, object_container &x) { generic_reader_->read_value(id, x); }
  virtual void read(const char *id, primary_key_base &x) { generic_reader_->read_value(id, x); }

/// @endcond OOS_DEV
  
private:
  T *generic_reader_;
};

template < typename T >
void read_many_to_many(object_reader &/*reader*/, const std::string &/*relation*/, const std::string &/*id*/, T &/*container*/)
{

}

}

#endif /* OBJECTATOMIZER_HPP */
