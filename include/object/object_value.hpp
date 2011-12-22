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

/**
 * @file object_value.hpp
 * @brief Contains the object_value class.
 *
 * This file contains the object_value class. The
 * class is used to update or retrieve a parameter
 * value of an object in a generic way (identified by
 * the name of the parameter field).
 * For an easy access global functions are given, wrapping
 * the creation of an instance of the object_value class
 * and the update or retrieve operation.
 */

namespace oos {

/**
 * @class object_value
 * @brief Update and retrieve values of an object.
 * @tparam T The type of the value.
 *
 * The object_value class updates and retrieves a single
 * value of an object in a generic way. Therefor the name
 * of the value must be known.
 * The name is given in the read_from and write_to methods
 * of each and every object_store object.
 */
template < class T >
class object_value : public object_atomizer
{
public:
  /**
   * Creates an object_value instance.
   */
  object_value()
    : object_(0)
    , succeeded_(false)
  {}

  virtual ~object_value() {}

  /**
   * @brief Sets a value of an object.
   *
   * This method try to set the value for parameter
   * of the given object identified by the given name.
   * Once the parameter is found and updated the operation
   * is flagged is succeeded.
   *
   * @param o The object the value should be set for.
   * @param id The identifier string for the parameter.
   * @param value The new value for the parameter.
   */
  void set(object *o, const char *id, const T &value)
  {
    value_ = value;
    id_ = id;
    object_ = o;
    succeeded_ = false;
    o->read_from(this);
    object_ = 0;
  }

  /**
   * @brief Gets a value of an object.
   *
   * This method try to get the value for a parameter
   * of the given object identified by the given name.
   * Once the parameter is found and retrieved the operation
   * is flagged is succeeded.
   *
   * @param o The object the value should be get for.
   * @param id The identifier string for the parameter.
   * @return The current value of the requested parameter.
   */
  T get(const object *o, const char *id)
  {
    id_ = id;
    succeeded_ = false;
    o->write_to(this);
    return value_;
  }

  /**
   * Returns true if the update or retrieve
   * was successful.
   *
   * @return True on successfull update or retrieve.
   */
  bool success() const
  {
    return succeeded_;
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
    retrieve<char, T>(id, x, value_);
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
    retrieve<float, T>(id, x, value_);
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
    retrieve<double, T>(id, x, value_);
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
    retrieve<int, T>(id, x, value_);
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
    retrieve<long, T>(id, x, value_);
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
    retrieve<unsigned, T>(id, x, value_);
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
    retrieve<bool, T>(id, x, value_);
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
    retrieve<const char*, T>(id, x, value_);
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
    retrieve<const std::string&, T>(id, x, value_);
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
    retrieve<const object_base_ptr&, T>(id, x, value_);
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
    update<T, char>(id, object_, value_, x);
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
    update<T, float>(id, object_, value_, x);
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
    update<T, double>(id, object_, value_, x);
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
    update<T, int>(id, object_, value_, x);
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
    update<T, long>(id, object_, value_, x);
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
    update<T, unsigned>(id, object_, value_, x);
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
    update<T, bool>(id, object_, value_, x);
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
    update<T, char*>(id, object_, value_, x);
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
    update<const T&, std::string>(id, object_, value_, x);
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
    update<T, object_base_ptr>(id, object_, value_, x);
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

  template < class X, class Y >
  void update(const char*, object*, X, Y&) {}

  template <>
  void update<T, T>(const char *id, object *o, T master, T &slave)
  {
    if (id_ != id) {
      return;
    }
    o->mark_modified();
    slave = master;
    succeeded_ = true;
  }

  template <>
  void update<const T&, T>(const char *id, object *o, const T &master, T &slave)
  {
    if (id_ != id) {
      return;
    }
    o->mark_modified();
    slave = master;
    succeeded_ = true;
  }

  template <>
  void update<const T&, object_base_ptr>(const char *id, object *o, const T &master, object_base_ptr &slave)
  {
    if (id_ != id) {
      return;
    }
    o->mark_modified();
    slave.reset(master.ptr());
    succeeded_ = true;
  }

  template <>
  void update<object*, object_base_ptr>(const char *id, object *o, object *master, object_base_ptr &slave)
  {
    if (id_ != id) {
      return;
    }
    o->mark_modified();
    slave.reset(master);
    succeeded_ = true;
  }

  template < class X, class Y >
  void retrieve(const char*, X, Y&) {}
  
  template <>
  void retrieve<T, T>(const char *id, T master, T &slave)
  {
    if (id_ != id) {
      return;
    }
    slave = master;
    succeeded_ = true;
  }

  template <>
  void retrieve<const T&, T>(const char *id, const T& master, T &slave)
  {
    if (id_ != id) {
      return;
    }
    slave = master;
    succeeded_ = true;
  }
    
  template <>
  void retrieve<const object_base_ptr&, object*>(const char *id, const object_base_ptr& master, object *&slave)
  {
    if (id_ != id) {
      return;
    }
    slave = master.ptr();
    succeeded_ = true;
  }
    
private:
  T value_;
  std::string id_;
  object *object_;
  bool succeeded_;
};

/**
 * @brief Updates parameter value of an object.
 * 
 * Updates the parameter identified by the given
 * name of the given object with the new given value.
 * If operation succeeded the function returns true.
 *
 * @tparam T Type of the value.
 * @param o The object_base_ptr to be updated.
 * @param field The identifier of the field.
 * @param value The new value of the parameter.
 * @return True if the operation succeeded.
 */
template < typename T >
bool
update_value(object_base_ptr &o, const char *field, const T &value)
{
  static object_value<T> type_value;
  type_value.set(o.ptr(), field, value);
  return type_value.success();
}

/**
 * @brief Updates parameter value of an object.
 * 
 * Updates the parameter identified by the given
 * name of the given object with the new given value.
 * If operation succeeded the function returns true.
 *
 * @tparam T Type of the value.
 * @param o The object to be updated.
 * @param field The identifier of the field.
 * @param value The new value of the parameter.
 * @return True if the operation succeeded.
 */
template < typename T >
bool
update_value(object *o, const char *field, const T &value)
{
  static object_value<T> type_value;
  type_value.set(o, field, value);
  return type_value.success();
}

/**
 * @brief Updates object_base_prt parameter value of an object.
 * 
 * Updates the parameter identified by the given
 * name of the given object with the new given value.
 * If operation succeeded the function returns true.
 *
 * @tparam T Type of the value.
 * @param o The object_base_ptr to be updated.
 * @param field The identifier of the field.
 * @param value The new value of the parameter.
 * @return True if the operation succeeded.
 */
template < typename T >
bool
update_value(object_base_ptr &o, const char *field, const object_ptr<T> &value)
{
  static object_value<object*> obj_value;
  obj_value.set(o.ptr(), field, value.ptr());
  return obj_value.success();
}

/**
 * @brief Retrieves a parameter value of an object.
 * 
 * Retrieves the parameter identified by the given
 * name of the given object. If operation succeeded
 * the function returns true.
 *
 * @tparam T Type of the value.
 * @param o The object_base_ptr.
 * @param field The identifier of the field.
 * @param value The placeholder for the requested value.
 * @return True if the operation succeeded.
 */
template < typename T >
bool
retrieve_value(const object_base_ptr &o, const char *field, T &value)
{
  static object_value<T> type_value;
  value = type_value.get(o.ptr(), field);
  return type_value.success();
}

/**
 * @brief Retrieves a parameter value of an object.
 * 
 * Retrieves the parameter identified by the given
 * name of the given object. If operation succeeded
 * the function returns true.
 *
 * @tparam T Type of the value.
 * @param o The object.
 * @param field The identifier of the field.
 * @param value The placeholder for the requested value.
 * @return True if the operation succeeded.
 */
template < typename T >
bool
retrieve_value(const object *o, const char *field, T &value)
{
  static object_value<T> type_value;
  value = type_value.get(o, field);
  return type_value.success();
}

/**
 * @brief Retrieves a parameter value of an object.
 * 
 * Retrieves the parameter identified by the given
 * name of the given object. If operation succeeded
 * the function returns true.
 *
 * @tparam T Type of the value.
 * @param o The object_base_ptr.
 * @param field The identifier of the field.
 * @param value The placeholder for the requested value.
 * @return True if the operation succeeded.
 */
template < typename T >
bool
retrieve_value(const object_base_ptr &o, const char *field, object_ptr<T> &value)
{
  static object_value<object*> obj_value;
  value.reset(obj_value.get(o.ptr(), field));
  return obj_value.success();
}

}

#endif /* OBJECT_VALUE_HPP */
