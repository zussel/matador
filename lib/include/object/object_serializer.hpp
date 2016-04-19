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
#include "tools/varchar.hpp"
#include "tools/access.hpp"
#include "tools/identifier.hpp"

#include "object/has_one.hpp"
#include "object/basic_has_many.hpp"

#include <string>
#include <cstring>

namespace oos {

class object_holder;
class varchar_base;

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
  object_serializer() {}

  virtual ~object_serializer();

  /**
   * Serialize the given serializable to the given buffer
   *
   * @param o The serializable to serialize.
   * @param buffer The byte_buffer to serialize to.
   * @return True on success.
   */
  template < class T >
  void serialize(T *o, byte_buffer *buffer)
  {
    restore = false;
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
    restore = true;
    ostore_ = ostore;
    buffer_ = buffer;
    oos::access::serialize(*this, *o);
    buffer_ = nullptr;
    ostore_ = nullptr;
  }

public:
  template < class T >
  void serialize(T &obj)
  {
    oos::access::serialize(*this, obj);
  }

  template < class T >
  void serialize(const char*, T &x)
  {
    if (restore) {
      buffer_->release(&x, sizeof(x));
    } else {
      buffer_->append(&x, sizeof(x));
    }
  }

	void serialize(const char* id, char *c, size_t s);
	void serialize(const char* id, std::string &s);

  template < unsigned int C >
  void serialize(const char*, varchar<C> &s)
  {
    if (restore) {
      size_t len = 0;
      buffer_->release(&len, sizeof(len));
      char *str = new char[len];
      buffer_->release(str, len);
      s.assign(str, len);
      delete [] str;
    } else {
      size_t len = s.size();

      buffer_->append(&len, sizeof(len));
      buffer_->append(s.str().c_str(), len);
    }
  }

	void serialize(const char* id, date &x);
	void serialize(const char* id, time &x);

  void serialize(const char *id, basic_identifier &x);

  template < class V >
  void serialize(const char* id, identifier<V> &x)
  {
    if (restore) {
      V val;
      serialize(id, val);
      x.value(val);
    } else {
      V val(x.value());
      serialize(id, val);
    }
  }

  template < class T >
	void serialize(const char* id, has_one<T> &x, cascade_type cascade)
  {
    if (restore) {
      /***************
       *
       * extract id and type of serializable from buffer
       * try to find serializable on serializable store
       * if found check type if wrong type throw error
       * else create serializable and set extracted id
       * insert serializable into serializable store
       *
       ***************/
      // Todo: correct implementation
      unsigned long oid = 0;
      serialize(id, oid);
      std::string type;
      serialize(id, type);

      if (oid > 0) {
        object_proxy *oproxy = find_proxy(oid);
        if (!oproxy) {
          oproxy =  new object_proxy(new T, oid, ostore_);
          insert_proxy(oproxy);
        }
        x.reset(oproxy, cascade);
      } else {
        x.reset(new object_proxy(new T, oid, ostore_), cascade);
      }
    } else {
      unsigned long oid = x.id();
      serialize(id, oid);
      serialize(id, const_cast<char*>(x.type()), strlen(x.type()));
    }
  }

  template<class T, template<class ...> class C>
  void serialize(const char *id, basic_has_many<T, C> &x, const char *, const char *)
  {
    if (restore) {
      typename basic_has_many<T, C>::size_type s = 0;
      // deserialize container size
      serialize(id, s);

      x.reset();

      for (typename basic_has_many<T, C>::size_type i = 0; i < 0; ++i) {

        // deserialize all items
        // and append it to container
        has_one<typename basic_has_many<T, C>::item_type> ptr;
        serialize(nullptr, ptr, cascade_type::NONE);

        // Todo: add method
//        x.insert(ptr);

        x.append(ptr);
      }


    } else {
      typename basic_has_many<T, C>::size_type s = x.size();
      serialize(id, s);

      typename basic_has_many<T, C>::iterator first = x.begin();
      typename basic_has_many<T, C>::iterator last = x.end();
      while (first != last) {
//        serialize(nullptr, first.relation_item(), cascade_type::NONE);
        typename basic_has_many<T, C>::relation_type rptr = (first++).relation_item();
        unsigned long oid = rptr.id();
        serialize(nullptr, oid);
        serialize(nullptr, const_cast<char*>(rptr.type()), strlen(rptr.type()));
      }
    }
  }

private:
  object_proxy* find_proxy(unsigned long oid);
  void insert_proxy(object_proxy *proxy);

private:
  object_store *ostore_ = nullptr;
  byte_buffer *buffer_ = nullptr;
  bool restore = false;
  basic_identifier_serializer basic_identifier_serializer_;
};
/// @endcond
}

#endif /* OBJECT_SERIALIZER_HPP */
