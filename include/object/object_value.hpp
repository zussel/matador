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

#ifndef OBJECT_VALUE_HPP
#define OBJECT_VALUE_HPP

#include "object/object_atomizer.hpp"
#include "object/object_ptr.hpp"
#include "object/object.hpp"

#include <string>

namespace oos {

template < class T >
class object_value : public object_atomizer
{
public:
  object_value()
    : succeeded_(false)
  {}
  virtual ~object_value() {}

  void set(object *o, const char *id, T value)
  {
    value_ = value;
    id_ = id;
    o->read_from(this);
  }

  T get(const object *o, const char *id)
  {
    id_ = id;
    o->write_to(this);
    return value_;
  }

  bool succeeded() const
  {
    return succeeded;
  }
private:
  /**
   * @brief Get character of object.
   * 
   * Get character identified by id of object .
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write_char(const char *id, char x)
  {
    sync<char>(id, x, value_);
  }

  /**
   * @brief Get float value of object.
   * 
   * Get float value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write_float(const char *id, float x)
  {
    sync<float>(id, x, value_);
  }

  /**
   * @brief Get double value of object.
   * 
   * Get double value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write_double(const char *id, double x)
  {
    sync<double>(id, x, value_);
  }

  /**
   * @brief Get int value of object.
   * 
   * Get int value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write_int(const char *id, int x)
  {
    sync<int>(id, x, value_);
  }

  /**
   * @brief Get long value of object.
   * 
   * Get long value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write_long(const char *id, long x)
  {
    sync<long>(id, x, value_);
  }

  /**
   * @brief Get unsigned value of object.
   * 
   * Get unsigned value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write_unsigned(const char *id, unsigned x)
  {
    sync<unsigned>(id, x, value_);
  }

  /**
   * @brief Get bool value of object.
   * 
   * Get bool value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write_bool(const char *id, bool x)
  {
    sync<bool>(id, x, value_);
  }

  /**
   * @brief Get const char pointer value of object.
   * 
   * Get const char pointer value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write_charptr(const char *id, const char* x)
  {
    sync<const char*>(id, x, value_);
  }

  /**
   * @brief Get string value of object.
   * 
   * Get string value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write_string(const char *id, const std::string &x)
  {
    sync<const std::string&>(id, x, value_);
  }

  /**
   * @brief Get object_base_ptr of object.
   * 
   * Get object_base_ptr identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write_object(const char *id, const object_base_ptr &x)
  {
    sync<const object_base_ptr&>(id, x, value_);
  }

  /**
   * @brief Get object_list_base of object.
   * 
   * Get object_list_base identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write_object_list(const char *, const object_list_base &)
  {
  }

  /**
   * @brief Get object_vector_base of object.
   * 
   * Get object_vector_base identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write_object_vector(const char *, const object_vector_base &)
  {
  }

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
  virtual void read_char(const char *id, char &x)
  {
    sync<char>(id, x, value_);
  }

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
  virtual void read_float(const char *id, float &x)
  {
    sync<float>(id, x, value_);
  }

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
  virtual void read_double(const char *id, double &x)
  {
    sync<double>(id, x, value_);
  }

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
	virtual void read_int(const char *id, int &x)
  {
    sync<int>(id, x, value_);
  }

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
	virtual void read_long(const char *id, long &x)
  {
    sync<long>(id, x, value_);
  }

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
	virtual void read_unsigned(const char *id, unsigned &x)
  {
    sync<unsigned>(id, x, value_);
  }

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
	virtual void read_bool(const char *id, bool &x)
  {
    sync<bool>(id, x, value_);
  }

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
	virtual void read_charptr(const char *id, char* &x)
  {
    sync<char*>(id, x, value_);
  }

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
	virtual void read_string(const char *id, std::string &x)
  {
    sync<const std::string&>(id, value_, x);
  }

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
	virtual void read_object(const char *id, object_base_ptr &x)
  {
    sync<object_base_ptr&>(id, x, value_);
  }

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
	virtual void read_object_list(const char *, object_list_base &)
  {
  }

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
	virtual void read_object_vector(const char *, object_vector_base &)
  {
  }

  template < class Y >
  void sync(const char*, Y, T&)
  {
    succeeded_ = false;
  }
  
  void sync(const char *id, const std::string &master, std::string &slave)
  {
    if (id_ != id) {
      return;
    }
    slave = master;
    succeeded_ = true;
  }
  
private:
  T value_;
  std::string id_;
  bool succeeded_;
};

}

#endif /* OBJECT_VALUE_HPP */
