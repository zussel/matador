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

#ifdef _MSC_VER
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

#include "object/attribute_serializer.hpp"
#include "object/object_atomizer.hpp"
#include "object/serializable.hpp"
#include "object/primary_key.hpp"

#include "tools/enable_if.hpp"
#include "tools/varchar.hpp"

#include <cstring>
#include <type_traits>

namespace oos {

class object_proxy;
class object_store;

/// @cond OOS_DEV

template < class T > class object_ref;
template < class T > class object_ptr;

/// @endcond OOS_DEV

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
class OOS_API object : public serializable
{
	// don't allow copying
	object(const object&) = delete;
	object& operator=(const object&) = delete;
	
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
	
  virtual void deserialize(object_reader &deserializer);
  virtual void serialize(object_writer &serializer) const;

  /**
   * @brief Returns the unique identifier of the object.
   * 
   * This method returns the unique id of the object. These
   * id is first set when the object is inserted into the
   * object_store. On creation the value of the id is zero.
   * 
   * @return The unique id of the object.
   */
	unsigned long id() const;

  /**
   * @brief Sets the id of the object.
   * 
   * Sets the id of the object.
   * 
   * @param oid The new id of the object.
   */
	void id(unsigned long oid);

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
    attribute_reader<T> reader(name, val);
    deserialize(reader);
    return reader.success();
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
    attribute_writer<T> writer(name, val);
    serialize(writer);
    return writer.success();
  }

  /**
   * Gets the value of a member identified by
   * the given name. If the operation succeeds
   * true is returned.
   * 
   * @tparam T        The type of the value to retrieve.
   * @param name      The name of the member variable.
   * @param val       The reference where the value is assigned to.
   * @param precision The precision of the value to get.
   * @return          True if the operation succeeds.
   */
  template < class T >
  bool get(const std::string &name, T &val, int precision)
  {
    attribute_writer<T> writer(name, val, precision);
    serialize(writer);
    return writer.success();
  }

  /**
   * Print the object to a given stream
   *
   * @param os The stream to write the object on.
   * @param o The object to write
   * @return The modified stream.
   */
  friend OOS_API std::ostream& operator <<(std::ostream &os, const object &o);

private:
	friend class object_store;
  friend class object_deleter;
  friend class object_base_ptr;
  friend class object_serializer;
  friend class object_container;

  friend class table;
  friend class relation_filler;
  friend class query;
  friend class database;

	primary_key<unsigned long> id_;
};

}

#endif /* OBJECT_HPP */
