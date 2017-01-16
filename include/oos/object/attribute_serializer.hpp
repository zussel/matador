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

#ifndef ATTRIBUTE_SERIALIZER_HPP
#define ATTRIBUTE_SERIALIZER_HPP

#include "oos/object/object_ptr.hpp"

#include "oos/utils/identifier.hpp"
#include "oos/utils/varchar.hpp"
#include "oos/utils/string.hpp"

#include <stdexcept>
#include <type_traits>
#include <cstring>

namespace oos {

/// @cond OOS_DEV

class basic_attribute_serializer
{
public:
  bool success() const
  {
    return success_;
  }

protected:
  explicit basic_attribute_serializer(const std::string &id) : id_(id) {}

protected:
  std::string id_;
  bool success_ = false;
};

/**
 * @tparam T The type of the attribute to set.
 * @class attribute_reader
 * @brief Set an attribute value of an serializable.
 *
 * A given attribute value of a template type is
 * tried to set into an serializable. Therefor the attribute
 * with given name must be found and the value must
 * be convertible into the objects attribute.
 */
template < class T >
class attribute_reader : public basic_attribute_serializer
{
public:
  attribute_reader(const std::string &id, const T &from)
    : basic_attribute_serializer(id)
    , from_(from)
  {}

  template < class V >
  void serialize(const char *id, V &to, typename std::enable_if< std::is_arithmetic<T>::value && std::is_arithmetic<V>::value >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to = (V)from_;
    this->success_ = true;
  }

  template < class V >
  void serialize(const char *id, V &to, typename std::enable_if<!std::is_arithmetic<T>::value && std::is_same<T, V>::value >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to = from_;
    this->success_ = true;
  }
  template < class V >
  void serialize(const char *, V &, typename std::enable_if<(!std::is_arithmetic<T>::value || !std::is_arithmetic<V>::value) &&  !std::is_same<T, V>::value >::type* = 0) {}
  void serialize(const char *, char*, size_t) {}
  template < class HAS_ONE >
  void serialize(const char *, HAS_ONE &, cascade_type) {}
  template < class HAS_MANY >
  void serialize(const char *, HAS_MANY &, const char *, const char *) {}

private:
  const T &from_;
};

template < class T >
class attribute_reader<object_ptr<T>> : public basic_attribute_serializer
{
public:
  attribute_reader(const std::string &id, const object_ptr<T> &from)
    : basic_attribute_serializer(id)
    , from_(from)
  {}

  template < class V >
  void serialize(V &obj)
  {
    access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const char *, V &) {}
  void serialize(const char *, char*, std::size_t) {}

  template < class V >
  void serialize(const char *, belongs_to<V> &x, cascade_type, typename std::enable_if<std::is_same<V, T>::value>::type* = 0)
  {
    x = from_;
    this->success_ = true;
  }
  template < class V >
  void serialize(const char *, has_one<V> &x, cascade_type, typename std::enable_if<std::is_same<V, T>::value>::type* = 0)
  {
    x = from_;
    this->success_ = true;
  }

  template < class HAS_MANY >
  void serialize(const char *, HAS_MANY &, const char *, const char *) {}

private:
  const object_ptr<T> &from_;
};

template < class T, class Enabled = void >
class has_many_attribute_reader;

template < class T >
class has_many_attribute_reader<T, typename std::enable_if<!std::is_base_of<object_holder, T>::value>::type> : public basic_attribute_serializer
{
public:
  has_many_attribute_reader(const std::string &id, const T &from)
    : basic_attribute_serializer(id)
    , from_(from)
  {}

  template < class V >
  void serialize(V &obj)
  {
    access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const char *, V &) {}
  void serialize(const char *, char*, std::size_t) {}

  template < class HAS_ONE >
  void serialize(const char *, HAS_ONE &, cascade_type) { }

  template<class V, template <class ...> class C>
  void serialize(const char *, has_many<V, C> &x, const char *, const char *)
  {
    x.push_back(from_);
    this->success_ = true;
  }

private:
  const T &from_;
};

template < class T >
class has_many_attribute_reader<object_ptr<T>> : public basic_attribute_serializer
{
public:
  has_many_attribute_reader(const std::string &id, const object_ptr<T> &from)
    : basic_attribute_serializer(id)
    , from_(from)
  {}

  template < class V >
  void serialize(V &obj)
  {
    access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const char *, V &) {}
  void serialize(const char *, char*, std::size_t) {}

  template < class HAS_ONE >
  void serialize(const char *, HAS_ONE &, cascade_type) { }

  template<class V, template <class ...> class C>
  void serialize(const char *, has_many<V, C> &x, const char *, const char *)
  {
    x.push_back(from_);
    this->success_ = true;
  }

private:
  const object_ptr<T> &from_;
};

template <>
class attribute_reader<const char*> : public basic_attribute_serializer
{
public:
  attribute_reader(const std::string &id, const char* from)
    : basic_attribute_serializer(id)
    , from_(from)
    , len_(strlen(from))
  {}

  ~attribute_reader() {}

  template < class V >
  void serialize(const char *, V &) {}
  template < unsigned int C >
  void serialize(const char *id, varchar<C> &to)
  {
    if (id_ != id) {
      return;
    }
    to = from_;
    this->success_ = true;
  }
  void serialize(const char *id, std::string &to)
  {
    if (id_ != id) {
      return;
    }
    to = from_;
    this->success_ = true;
  }

  void serialize(const char *id, char *to, size_t len)
  {
    if (id_ != id) {
      return;
    }
    if (len > len_) {
#ifdef _MSC_VER
		strncpy_s(to, len, from_, len_);
		to[len_] = '\0';
#else
		strcpy(to, from_);
#endif
      this->success_ = true;
    }
  }
  template < class HAS_ONE >
  void serialize(const char *, HAS_ONE &, cascade_type) {}
  template < class HAS_MANY >
  void serialize(const char *, HAS_MANY &, const char *, const char *) {}

private:
  const char* from_ = nullptr;
  size_t len_ = 0;
};

template < class T >
class attribute_writer : public basic_attribute_serializer
{
public:
  attribute_writer(const std::string &id, T &to, size_t precision = 0)
    : basic_attribute_serializer(id)
    , to_(to)
    , precision_(precision)
  {}

  ~attribute_writer() {}

  template < class V >
  void serialize(const char *id, V &from, typename std::enable_if< std::is_arithmetic<T>::value && std::is_arithmetic<V>::value >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = (T)from;
    success_ = true;
  }

  template < class V >
  void serialize(const char *id, V &from, typename std::enable_if<!std::is_arithmetic<T>::value && std::is_same<T, V>::value >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = from;
    success_ = true;
  }
  template < class V >
  void serialize(const char *, V &, typename std::enable_if<(!std::is_arithmetic<T>::value || !std::is_arithmetic<V>::value) &&  !std::is_same<T, V>::value >::type* = 0) {}

  void serialize(const char *, char*, size_t) {}
  template < class HAS_ONE >
  void serialize(const char *, HAS_ONE &, cascade_type) {}
  template < class HAS_MANY >
  void serialize(const char *, HAS_MANY &, const char *, const char *) {}

private:
  T &to_;
  size_t precision_;
};

template < class T >
class attribute_writer<object_ptr<T>> : public basic_attribute_serializer
{
public:
  attribute_writer(const std::string &id, object_ptr<T> &to)
  : basic_attribute_serializer(id)
  , to_(to)
  {}

  template < class V >
  void serialize(const char *, V &) {}
  void serialize(const char *, char*, size_t) {}

  template < class V >
  void serialize(const char *, belongs_to<V> &x, cascade_type, typename std::enable_if<std::is_same<V, T>::value>::type* = 0)
  {
    to_ = x;
  }
  template < class V >
  void serialize(const char *, has_one<V> &x, cascade_type, typename std::enable_if<std::is_same<V, T>::value>::type* = 0)
  {
    to_ = x;
  }

  template < class HAS_MANY >
  void serialize(const char *, HAS_MANY &, const char *, const char *) {}

private:
  object_ptr<T> &to_;
};

template < class T, class Enabled = void >
class has_many_attribute_writer;

template < class T >
class has_many_attribute_writer<T, typename std::enable_if<!std::is_base_of<object_holder, T>::value>::type> : public basic_attribute_serializer
{
public:
  has_many_attribute_writer(const std::string &id, const T &to)
  : basic_attribute_serializer(id)
  , to_(to)
  {}

  template < class V >
  void serialize(V &obj)
  {
    access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const char *, V &) {}
  void serialize(const char *, char*, size_t) {}

  template < class HAS_ONE >
  void serialize(const char *, HAS_ONE &, cascade_type) {}

  template<class V, template <class ...> class C>
  void serialize(const char *, has_many<V, C> &x, const char *, const char *)
  {
    x.remove(to_);
    this->success_ = true;
  }
private:
  const T &to_;
};

template < class T >
class has_many_attribute_writer<object_ptr<T>> : public basic_attribute_serializer
{
public:
  has_many_attribute_writer(const std::string &id, const object_ptr<T> &to)
  : basic_attribute_serializer(id)
  , to_(to)
  {}

  template < class V >
  void serialize(V &obj)
  {
    access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const char *, V &) {}
  void serialize(const char *, char*, size_t) {}

  template < class HAS_ONE >
  void serialize(const char *, HAS_ONE &, cascade_type) {}

  template<class V, template <class ...> class C>
  void serialize(const char *, has_many<V, C> &x, const char *, const char *)
  {
    x.remove(to_);
    this->success_ = true;
  }
private:
  const object_ptr<T> &to_;
};

template <>
class attribute_writer<std::string> : public basic_attribute_serializer
{
public:
  attribute_writer(const std::string &id, std::string &to, size_t precision = 0)
    : basic_attribute_serializer(id)
    , to_(to)
    , precision_(precision)
  {}

  ~attribute_writer() {}

  template < class V >
  void serialize(const char *id, V &from, typename std::enable_if< !std::is_floating_point<V>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = std::to_string(from);
    success_ = true;
  }

  template < class V >
  void serialize(const char *id, V &from, typename std::enable_if< std::is_floating_point<V>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = oos::to_string(from, precision_);
    success_ = true;
  }

  void serialize(const char *id, std::string &from)
  {
    if (id_ != id) {
      return;
    }
    to_ = from;
    success_ = true;
  }

  template < unsigned int C >
  void serialize(const char *id, oos::varchar<C> &from)
  {
    if (id_ != id) {
      return;
    }
    to_ = from.str();
    success_ = true;
  }

  void serialize(const char *id, char *from, size_t len)
  {
    if (id_ != id) {
      return;
    }
    to_.assign(from, len);
    success_ = true;
  }

  void serialize(const char *id, date &from)
  {
    if (id_ != id) {
      return;
    }
    to_ = to_string(from);
    success_ = true;
  }
  void serialize(const char *id, oos::time &from)
  {
    if (id_ != id) {
      return;
    }
    to_ = to_string(from);
    success_ = true;
  }

  template < class V >
  void serialize(const char *id, identifier<V> &x)
  {
    if (id_ != id) {
      return;
    }
    std::stringstream to;
    x.print(to);
    to_ = to.str();
    success_ = true;
  }

  template < class V >
  void serialize(const char *id, belongs_to<V> &x, cascade_type)
  {
    if (id_ != id) {
      return;
    }
    std::stringstream to;
    if (x.has_primary_key()) {
      x.primary_key()->print(to);
    } else {
      to << x.id();
    }
    to_ = to.str();
    success_ = true;
  }

  template < class V >
  void serialize(const char *id, has_one<V> &x, cascade_type)
  {
    if (id_ != id) {
      return;
    }
    std::stringstream to;
    if (x.has_primary_key()) {
      x.primary_key()->print(to);
    } else {
      to << x.id();
    }
    to_ = to.str();
    success_ = true;
  }

  void serialize(const char*, abstract_has_many&, const char*, const char*) {}

private:
  std::string &to_;
  size_t precision_;
};

template <>
class attribute_writer<char*>
{
public:
  /**
   * @brief Creates an attribute_writer
   *
   * Creates an attribute_writer for an attribute id of type T
   * where id is the name of the attribute.
   *
   * @tparam T The type of the attribute.
   * @param id The name of the attribute.
   * @param to The attribute value to retrieve.
   */
  attribute_writer(const std::string &id, char *to, size_t size/*, size_t precision = 0*/)
    : id_(id)
    , to_(to)
    , size_(size)
    , success_(false)
//    , precision_(precision)
  {}

  ~attribute_writer() {}

  /**
   * @brief True if value could be retrieved.
   *
   * Returns true if the value could
   * be retrieved successfully.
   *
   * @return True if value could be retrieved.
   */
  bool success() const
  {
    return success_;
  }

  template < class V >
  void serialize(const char *id, V &/*from*/)
  {
    if (id_ != id) {
      return;
    }
    // Todo: convert each type to char*
    success_ = true;
  }

  void serialize(const char*, date&) {}
  void serialize(const char*, time&) {}
  template < class V >
  void serialize(const char*, belongs_to<V> &, cascade_type) {}
  template < class V >
  void serialize(const char*, has_one<V> &, cascade_type) {}
  void serialize(const char*, abstract_has_many&, const char*, const char*) {}
  template < class V >
  void serialize(const char*, const identifier<V> &) {}

  void serialize(const char *id, char *from, size_t size)
  {
    if (id_ != id) {
      return;
    }
    if (size_ < size) {
      // not enough size
      return;
    }
#ifdef _MSC_VER
    strncpy_s(to_, size_, from, size);
#else
    strncpy(to_, from, size);
#endif
    size_t from_size = strlen(from);
    if (from_size > (size_t)size) {
      to_[size-1] = '\0';
    } else {
      to_[from_size] = '\0';
    }
    success_ = true;
  }

private:
  std::string id_;
  char *to_;
  size_t size_;
  bool success_;
//  size_t precision_;
};

/// @endcond

}

#endif /* ATTRIBUTE_SERIALIZER_HPP */
