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

#ifndef OBJECT_SERIALIZER_HPP
#define OBJECT_SERIALIZER_HPP

#ifdef _MSC_VER
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_API
#endif

#include "tools/byte_buffer.hpp"
#include "object/object_atomizer.hpp"

#include <string>

namespace oos {

class serializable;
class object_base_ptr;
class object_store;
class object_proxy;
class byte_buffer;
class varchar_base;
class object_container;

/**
 * @cond OOS_DEV
 * @class object_serializer
 * @brief serialize objects to a given buffer.
 *
 * The object_serializer class serialize a given
 * serializable to a given buffer or deserialize a given
 * serializable from a given buffer.
 * On deserialization the class doesn't take care
 * of the correctness of the serializable and the current
 * memory the buffer points to.
 * The application is responsible for this correctness.
 */
class OOS_API object_serializer
  : public generic_object_reader<object_serializer>
  , public generic_object_writer<object_serializer>
{
public:
  /**
   * Creates an object_serializer
   */
  object_serializer()
    : generic_object_reader<object_serializer>(this)
    , generic_object_writer<object_serializer>(this)
    , ostore_(NULL)
    , buffer_(NULL)
  {}

  virtual ~object_serializer();

  /**
   * Serialize the given serializable to the given buffer
   *
   * @param o The serializable to serialize.
   * @param buffer The byte_buffer to serialize to.
   * @return True on success.
   */
  bool serialize(const serializable *o, byte_buffer *buffer);

  /**
   * Serialize the given serializable to the given buffer
   *
   * @param o The serializable to deserialize.
   * @param buffer The byte_buffer to deserialize from.
   * @param ostore The object_store where the serializable resides.
   * @return True on success.
   */
  bool deserialize(serializable *o, byte_buffer *buffer, object_store *ostore);

public:
  template < class T >
  void write_value(const char*, const T &x)
  {
    buffer_->append(&x, sizeof(x));
  }

	void write_value(const char* id, const char *c, int s);
	void write_value(const char* id, const std::string &s);
  void write_value(const char*, const varchar_base &s);
	void write_value(const char* id, const date &x);
	void write_value(const char* id, const time &x);
	void write_value(const char* id, const object_base_ptr &x);
	void write_value(const char* id, const object_container &x);
	void write_value(const char* id, const primary_key_base &);

  template < class T >
  void read_value(const char*, T &x)
  {
    buffer_->release(&x, sizeof(x));
  }

	void read_value(const char* id, char *&c, int s);
	void read_value(const char* id, std::string &s);
  void read_value(const char*, varchar_base &s);
  void read_value(const char* id, date &x);
  void read_value(const char* id, time &x);
  void read_value(const char* id, object_base_ptr &x);
	void read_value(const char* id, object_container &x);
	void read_value(const char* id, primary_key_base &x);
  
  void write_object_container_item(const object_proxy *proxy);

private:
  object_store *ostore_;
  byte_buffer *buffer_;
};
/// @endcond
}

#endif /* OBJECT_SERIALIZER_HPP */
