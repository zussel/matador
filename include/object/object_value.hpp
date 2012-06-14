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
#include "object/object_ptr.hpp"

#include "tools/varchar.hpp"

#include <cstring>
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

template < class T >
class object_value;

/// @cond OOS_DEV

namespace detail {
  template < typename X, typename Y >
  struct updater
  {
    void update(const char*, const char*, bool&, object_value<X>&, object*, X, Y&) {}
  };
  
  template < typename T >
  struct updater<T, T>
  {
    void update(const char *id, const char *f, bool &r, object_value<T> &ov, object *o, T master, T &slave);
  };

  template < typename T >
  struct updater<T*, object_base_ptr>
  {
    void update(const char *id, const char *f, bool &r, object_value<T*> &ov, object *o, T *master, object_base_ptr &slave);
  };

  template < typename T >
  struct updater<T, varchar_base&>
  {
    void update(const char *id, const char *f, bool &r, object_value<T> &ov, object *o, T master, varchar_base &slave) {}
  };

  template < >
  struct OOS_API updater<std::string, varchar_base>
  {
    void update(const char *id, const char *f, bool &r, object_value<std::string> &ov, object *o, const std::string &master, varchar_base &slave);
  };

  template < unsigned int C >
  struct updater<varchar<C>&, varchar_base&>
  {
    void update(const char *id, const char *f, bool &r, object_value<varchar<C> > &ov, object *o, varchar<C> &master, varchar_base &slave);
  };

  template < >
  struct OOS_API updater<varchar_base&, varchar_base&>
  {
    void update(const char *id, const char *f, bool &r, object_value<varchar_base> &ov, object *o, varchar_base &master, varchar_base &slave);
  };

  template < typename X, typename Y >
  struct retriever
  {
    void retrieve(const char*, const char*, bool&, X, Y&) {}
  };

  template < typename X >
  struct retriever<X, X>
  {
    void retrieve(const char *id, const char *f, bool &r, X master, X &slave)
    {
      if (strcmp(id, f) != 0) {
        return;
      }
      slave = master;
      r = true;
    }
  };

  template < typename X >
  struct retriever<const X&, X>
  {
    void retrieve(const char *id, const char *f, bool &r, const X &master, X &slave)
    {
      if (strcmp(id, f) != 0) {
        return;
      }
      slave = master;
      r = true;
    }
  };

  template < typename X >
  struct retriever<const object_base_ptr&, X*>
  {
    void retrieve(const char *id, const char *f, bool &r, const object_base_ptr& master, X *&slave)
    {
      if (strcmp(id, f) != 0) {
        return;
      }
      slave = master.ptr();
      r = true;
    }
  };

  template < typename Y >
  struct retriever<const varchar_base&, Y>
  {
    void retrieve(const char*, const char*, bool&, const varchar_base&, Y&) {}
  };

  template < >
  struct retriever<const varchar_base&, std::string>
  {
    void retrieve(const char *id, const char *f, bool &r, const varchar_base& master, std::string& slave)
    {
      if (strcmp(id, f) != 0) {
        return;
      }
      slave = master.str();
      r = true;
    }
  };

  template < >
  struct retriever<const varchar_base&, varchar_base>
  {
    void retrieve(const char *id, const char *f, bool &r, const varchar_base& master, varchar_base& slave)
    {
      if (strcmp(id, f) != 0) {
        return;
      }
      slave = master;
      r = true;
    }
  };

  template < unsigned int C >
  struct retriever<const varchar_base&, varchar<C> >
  {
    void retrieve(const char *id, const char *f, bool &r, const varchar_base& master, varchar<C>& slave)
    {
      if (strcmp(id, f) != 0) {
        return;
      }
      slave = master.str();
      r = true;
    }
  };
}
/// @endcond

/// @cond OOS_DEV
class OOS_API object_value_base : public object_atomizer
{
public:
  virtual ~object_value_base();
  
protected:
  template < typename X, typename Y > friend struct detail::updater;

  void update(object *o);
  void retrieve(const object *o);
  void mark_modified(object *o);
};
/// @endcond

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
class object_value : public object_value_base
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
    update(o);
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
    retrieve(o);
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
	virtual void write(const char *id, char x)
  {
    detail::retriever<char, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get float value of object.
   * 
   * Get float value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write(const char *id, float x)
  {
    detail::retriever<float, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get double value of object.
   * 
   * Get double value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write(const char *id, double x)
  {
    detail::retriever<double, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get short value of object.
   * 
   * Get short value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write(const char *id, short x)
  {
    detail::retriever<int, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get int value of object.
   * 
   * Get int value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write(const char *id, int x)
  {
    detail::retriever<int, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get long value of object.
   * 
   * Get long value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write(const char *id, long x)
  {
    detail::retriever<long, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get unsigned short value of object.
   * 
   * Get unsigned short value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write(const char *id, unsigned short x)
  {
    detail::retriever<unsigned short, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get unsigned int value of object.
   * 
   * Get unsigned int value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
  virtual void write(const char *id, unsigned int x)
  {
    detail::retriever<unsigned int, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get unsigned long value of object.
   * 
   * Get unsigned long value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write(const char *id, unsigned long x)
  {
    detail::retriever<unsigned long, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get bool value of object.
   * 
   * Get bool value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write(const char *id, bool x)
  {
    detail::retriever<bool, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get const char pointer value of object.
   * 
   * Get const char pointer value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write(const char *id, const char* x)
  {
    detail::retriever<const char*, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get string value of object.
   * 
   * Get string value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write(const char *id, const std::string &x)
  {
    detail::retriever<const std::string&, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get varchar value of object.
   * 
   * Get varchar value identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write(const char *id, const varchar_base &x)
  {
    detail::retriever<const varchar_base&, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

  /**
   * @brief Get object_base_ptr of object.
   * 
   * Get object_base_ptr identified by id of object.
   * 
   * @param id Unique id of the data.
   * @param x The data to the value.
   */
	virtual void write(const char *id, const object_base_ptr &x)
  {
    detail::retriever<const object_base_ptr&, T> r;
    r.retrieve(id, id_.c_str(), succeeded_, x, value_);
  }

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
  virtual void read(const char *id, char &x)
  {
    static detail::updater<T, char> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

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
  virtual void read(const char *id, float &x)
  {
    static detail::updater<T, float> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

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
  virtual void read(const char *id, double &x)
  {
    static detail::updater<T, double> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

  /**
   * @brief Read an short from the atomizer.
   * 
   * Read an short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, short &x)
  {
    static detail::updater<T, short> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

  /**
   * @brief Read an integer from the atomizer.
   * 
   * Read an integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, int &x)
  {
    static detail::updater<T, int> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

  /**
   * @brief Read a long from the atomizer.
   * 
   * Read a long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, long &x)
  {
    static detail::updater<T, long> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

  /**
   * @brief Read an unsigned short from the atomizer.
   * 
   * Read a unsigned short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, unsigned short &x)
  {
    static detail::updater<T, unsigned short> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

  /**
   * @brief Read an unsigned int from the atomizer.
   * 
   * Read a unsigned int from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, unsigned int &x)
  {
    static detail::updater<T, unsigned int> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

  /**
   * @brief Read an unsigned long from the atomizer.
   * 
   * Read a unsigned long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, unsigned long &x)
  {
    static detail::updater<T, unsigned long> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

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
	virtual void read(const char *id, bool &x)
  {
    static detail::updater<T, bool> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

  /**
   * @fn virtual void read(const char *id, char *&x)
   * @brief Read a const char pointer from the atomizer.
   * 
   * Read a const char pointer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, char* &x)
  {
    static detail::updater<T, char*> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

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
	virtual void read(const char *id, std::string &x)
  {
    static detail::updater<T, std::string> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

  /**
   * @brief Read a varchar from the atomizer.
   * 
   * Read a varchar from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, varchar_base &x)
  {
    static detail::updater<T, varchar_base> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

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
	virtual void read(const char *id, object_base_ptr &x)
  {
    static detail::updater<T, object_base_ptr> u;
    u.update(id, id_.c_str(), succeeded_, *this, object_, value_, x);
  }

private:
  T value_;
  std::string id_;
  object *object_;
  bool succeeded_;
};

/// @cond OOS_DEV

namespace detail {

template < typename T >
void
updater<T, T>::update(const char *id, const char *f, bool &r, object_value<T> &ov, object *o, T master, T &slave)
{
  if (strcmp(f, id) != 0) {
    return;
  }
  ov.mark_modified(o);
  slave = master;
  r = true;
}

template < typename T >
void
updater<T*, object_base_ptr>::update(const char *id, const char *f, bool &r, object_value<T*> &ov, object *o, T *master, object_base_ptr &slave)
{
  if (strcmp(f, id) != 0) {
    return;
  }
  ov.mark_modified(o);
  slave.reset(master);
  r = true;
}

template < unsigned int C >
void
updater<varchar<C>&, varchar_base&>::update(const char *id, const char *f, bool &r, object_value<varchar<C> > &ov, object *o, varchar<C> &master, varchar_base &slave)
{
  if (strcmp(f, id) != 0) {
    return;
  }
  ov.mark_modified(o);
  slave = master;
  r = true;
}

}

/// @endcond

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
