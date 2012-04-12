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

#ifndef OBJECT_HPP
#define OBJECT_HPP

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

#include "object/object_value.hpp"
#include "object/object_atomizable.hpp"

#include <iostream>

namespace oos {

struct object_proxy;
class object_store;
//class object_atomizer;

/**
 * @class object
 * @brief The base class for all objects.
 * 
 * The object class must be the base class
 * of all classes inserted into the object_store.
 * At least the object_store handles with objects
 * of base type object. When iterating over a view
 * these objects are casted to the concrete type.
 * 
 * The object is identified by a unique id, which is
 * set by the object_store.
 */
class OOS_API object : public object_atomizable
{
	// don't allow copying
	object(const object&);
	object& operator=(const object&);
	
public:
  /**
   * @brief Create a new object.
   * 
   * Creates a new object, which is not
   * member of the object_store.
   */
	object();

  /**
   * Destroys the object.
   */
	virtual ~object();
	
  /**
   * @brief Interface to read into object members
   * 
   * The read_from method is used to accept
   * all members of an object. It is used
   * to change them.
   * 
   * @param a An object of type object_atomizer to read the object member values from.
   */
	virtual void read_from(object_atomizer *a);

  /**
   * @brief Interface to write from object members
   * 
   * The write to method reads all object members
   * and writes them to the given object_atomizer object.
   *
   * @param a An object of type object_atomizer to write the object member values to.
   */
	virtual void write_to(object_atomizer *a) const;

  /**
   * Returns the classname of the object
   * 
   * @return Name of the object type
   */
	const char* classname() const;
  
  /**
   * @brief Returns the unique identifier of the object.
   * 
   * This method returns the unique id of the object. These
   * id is first set when the object is inserted into the
   * object_store. On creation the value of the id is zero.
   * 
   * @return The unique id of the object.
   */
	long id() const;

  /**
   * @brief Sets the id of the object.
   * 
   * Sets the id of the object.
   * 
   * @param oid The new id of the object.
   */
	void id(long oid);

  /**
   * @brief Returns the object store.
   * 
   * Returns the object_store into which
   * this object was inserted. If this object
   * wasn't inserted NULL is returned.
   * 
   * @return The object_store to which the object belongs.
   */
  object_store* ostore() const;

  /**
   * Sets a value of a member identified by
   * the given name. If the operation succeeds
   * true is returned.
   * 
   * @tparam T     The type of the value to set.
   * @param name   The name of the member variable.
   * @param val    The new value for the member.
   * @return       True if the operation succeeds.
   */
  template < class T >
  bool set(const std::string &name, const T &val)
  {
    return update_value(this, name.c_str(), val);
  }

  /**
   * Gets the value of a member identified by
   * the given name. If the operation succeeds
   * true is returned.
   * 
   * @tparam T     The type of the value to retrieve.
   * @param name   The name of the member variable.
   * @param val    The reference where the value is assigned to.
   * @return       True if the operation succeeds.
   */
  template < class T >
  bool get(const std::string &name, T &val)
  {
    return retrieve_value(this, name.c_str(), val);
  }

  /**
   * Modify the attribute assigning
   * the new given value to attributes
   * reference.
   *
   * @tparam T Type of attribute to change.
   * @param attr Refernce to attribute to change.
   * @param val New value for attribute.
   */
  template < class T >
  void modify(T &attr, const T &val)
  {
    mark_modified();
    attr = val;
  }

  /**
   * Modify an object_ptr attribute assigning
   * the new given value to attributes reference.
   *
   * @tparam T Type of object_ptr to change.
   * @param attr Refernce to object_ptr to change.
   * @param val New value for object_ptr.
   */
  template < class T >
  void modify(oos::object_ref<T> &attr, const oos::object_ptr<T> &val)
  {
    mark_modified();
    attr = val;
  }

  /**
   * Modify a varchar_base attribute assigning
   * the new given value to attributes reference.
   *
   * @param attr Refernce to varchar_base to change.
   * @param val New value for varchar_base.
   */
  void modify(varchar_base &attr, const std::string &val)
  {
    mark_modified();
    attr = val;
  }

  /**
   * Modify a varchar_base attribute assigning
   * the new given value to attributes reference.
   *
   * @param attr Refernce to varchar_base to change.
   * @param val New value for varchar_base.
   */
  void modify(varchar_base &attr, const varchar_base &val)
  {
    mark_modified();
    attr = val;
  }

  /**
   * Print the object to a given stream
   *
   * @param os The stream to write the object on.
   * @param o The object to write
   * @return The modified stream.
   */
  friend OOS_API std::ostream& operator <<(std::ostream &os, const object &o);

protected:
  /**
   * @brief Returns the object_proxy of the object.
   * 
   * Returns the object_proxy of the object. If the
   * object isn't inserted into a object_store NULL
   * is returned.
   * 
   * @return The object_proxy of the object.
   */
  object_proxy* proxy() const { return proxy_; }

  /**
   * @brief Marks this object as modified in its object_store
   *
   * Marks this object as modified in its object_store.
   * This method must be called before modifing a derived
   * object. Otherwise the object can't be restored on an
   * error or rollback.
   */
	void mark_modified();

private:
	friend class object_store;
  friend class object_deleter;
  friend class object_base_ptr;
  friend class object_serializer;
  friend class object_list_base;
  friend class object_vector_base;
  friend class object_value_base;
	
	long id_;
  object_proxy *proxy_;
};

}

#endif /* OBJECT_HPP */
