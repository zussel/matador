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

#include "object/access.hpp"
#include "object/identifier.hpp"
#include "object/object_ptr.hpp"

#include <string>

namespace oos {

class basic_object_holder;
class object_store;
class object_proxy;
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
{
public:
  /**
   * Creates an object_serializer
   */
  object_serializer()
    : ostore_(NULL)
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
  template < class T >
  void serialize(const T *o, byte_buffer *buffer)
  {
    buffer_ = buffer;
    oos::access::serialize(*this, *o);
    buffer_ = nullptr;
  }

  /**
   * Serialize the given serializable to the given buffer
   *
   * @param o The serializable to deserialize.
   * @param buffer The byte_buffer to deserialize from.
   * @param ostore The object_store where the serializable resides.
   * @return True on success.
   */
  template < class T >
  void deserialize(T *o, byte_buffer *buffer, object_store *ostore)
  {
    ostore_ = ostore;
    buffer_ = buffer;
    oos::access::deserialize(*this, *o);
    buffer_ = nullptr;
    ostore_ = nullptr;
  }

public:
  template < class T >
  void serialize(const char*, const T &x)
  {
    buffer_->append(&x, sizeof(x));
  }

	void serialize(const char* id, const char *c, size_t s);
	void serialize(const char* id, const std::string &s);
  template < unsigned int C >
  void serialize(const char*, const varchar<C> &s)
  {
    size_t len = s.size();

    buffer_->append(&len, sizeof(len));
    buffer_->append(s.str().c_str(), len);
  }

	void serialize(const char* id, const date &x);
	void serialize(const char* id, const time &x);
  template < class T, bool TYPE >
	void serialize(const char* id, const object_holder<T, TYPE> &x)
  {
    serialize(id, x.id());
    serialize(id, x.type(), strlen(x.type()));
  }

	void serialize(const char* id, const object_container &x);
  template < class V >
	void serialize(const char* id, const identifier<V> &x)
  {
    serialize(id, x.value());
  }

  template < class T >
  void deserialize(const char*, T &x)
  {
    buffer_->release(&x, sizeof(x));
  }

  void deserialize(const char* id, char *c, size_t s);
	void deserialize(const char* id, std::string &s);
  template < unsigned int C >
  void deserialize(const char*, varchar<C> &s)
  {
    size_t len = 0;
    buffer_->release(&len, sizeof(len));
    char *str = new char[len];
    buffer_->release(str, len);
    s.assign(str, len);
    delete [] str;
  }
  void deserialize(const char* id, date &x);
  void deserialize(const char* id, time &x);
  void deserialize(const char* id, basic_object_holder &x);
	void deserialize(const char* id, object_container &x);
  template < class V >
  void deserialize(const char* id, identifier<V> &x)
  {
    V val;
    deserialize(id, val);
    x.value(val);
  }
  
  void write_object_container_item(const object_proxy *proxy);

private:
  object_store *ostore_;
  byte_buffer *buffer_;
};
/// @endcond
}

#endif /* OBJECT_SERIALIZER_HPP */
