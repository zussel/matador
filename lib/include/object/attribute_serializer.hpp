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

#include "object/object_ptr.hpp"
#include "object/object_container.hpp"
#include "object/identifier.hpp"

#include "tools/varchar.hpp"
#include "tools/string.hpp"

#include <stdexcept>
#include <type_traits>
#include <cstring>

namespace oos {

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

template < class T, class Enable = void >
class attribute_reader;

template < class T >
class attribute_reader<T, typename std::enable_if< !std::is_same<T, char*>::value >::type>
{
public:
  /**
   * @brief Creates an attribute_reader
   *
   * Creates an attribute_reader for an attribute id of type T
   * where id is the name of the attribute.
   *
   * @tparam T The type of the attribute.
   * @param id The name of the attribute.
   * @param from The attribute value to set.
   */
  attribute_reader(const std::string &id, const T &from)
    : id_(id)
    , from_(from)
    , success_(false)
  {}
  
  ~attribute_reader() {}

  /**
   * @brief True if value was set.
   *
   * Returns true if the value could
   * be set successfully.
   *
   * @return True if value was set.
   */
  bool success() const
  {
    return success_;
  }

public:
  template < class V >
  void serialize(const char *id, V &to, typename std::enable_if< std::is_same<T, V>::value >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to = from_;
    success_ = true;
  }

  template < class V >
  void serialize(const char *id, V &to, typename std::enable_if<
    std::is_base_of<V, T>::value &&
    std::is_same<V, oos::varchar_base>::value
  >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to = from_;
  }

  template < class V >
  void serialize(const char *id, V &/*to*/, typename std::enable_if<
    std::is_base_of<V, T>::value &&
    std::is_same<V, oos::basic_object_holder>::value
  >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
  }

  template < class V >
  void serialize(const char *id, V &/*to*/, typename std::enable_if<
    !std::is_same<T, V>::value &&
    !std::is_base_of<V, T>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    // Todo: throw exception
  }

  template < class V >
  void serialize(const char *id, identifier<V> &x)
  {
    V val;
    serialize(id, val);
    x.value(val);
  }

  void serialize(const char *id, char *, size_t)
  {
    if (id_ != id) {
      return;
    }

    // Todo: convert any type to char*
    success_ = true;
  }
  template < class V >
  void serialize(const char *id, date &to, typename std::enable_if<
    std::is_same<T, V>::value &&
      (std::is_same<V, oos::date>::value ||
       std::is_same<V, oos::time>::value
      )>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to = from_;
    success_ = true;
  }

  void serialize(const char*, object_container&) {}

private:
  std::string id_;
  const T &from_;
  bool success_;
};

#ifndef OOS_DOXYGEN_DOC
template < class T >
class attribute_reader<T, typename std::enable_if< std::is_same<T, char*>::value >::type>
{
public:
  attribute_reader(const std::string &id, const char from[], size_t size)
    : id_(id)
    , from_(from)
    , size_(size)
    , success_(false)
  {}

  /**
 * @brief True if value was set.
 *
 * Returns true if the value could
 * be set successfully.
 *
 * @return True if value was set.
 */
  bool success() const
  {
    return success_;
  }

  template < class V >
  void serialize(const char *id, V &/*to*/)
  {
    if (id_ != id) {
      return;
    }
    // convert from char* to any type
    // i.e. to = to_value<V>(from_);
    success_ = true;
  }

  void serialize(const char *id, char *to, size_t s)
  {
    if (id_ != id) {
      return;
    }
    if (s < size_) {
      return;
    }
#ifdef _MSC_VER
    strncpy_s(to, s, from_, size_);
#else
    strncpy(to, from_, size_);
#endif
    to[size_] = '\0';
    success_ = true;
  }
private:
  std::string id_;
  const char *from_;
  size_t size_;
  bool success_;
};
#endif

/**
 * @tparam T The type of the attribute to retieve.
 * @class attribute_writer
 * @brief Retrieve an attribute value from an serializable.
 *
 * A attribute value of a template type is
 * tried to retriev from an serializable. Therefor the attribute
 * of given name must be found and the value must
 * be convertible from the objects attribute.
 */
template < class T >
class attribute_writer
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
   * @param precision The precision of the value.
   */
  attribute_writer(const std::string &id, T &to, size_t precision = 0)
    : id_(id)
    , to_(to)
    , success_(false)
    , precision_(precision)
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

public:
  template < class V >
  void serialize(const char *id, const V &from, typename std::enable_if<
    std::is_same<T, V>::value &&
    !std::is_floating_point<V>::value
  >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = from;
    success_ = true;
  }

  template < class V >
  void serialize(const char *id, const V &from, typename std::enable_if<
    std::is_same<T, V>::value &&
    std::is_floating_point<V>::value
  >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = from;
    success_ = true;
  }

  template < class V >
  void serialize(const char *id, const V &from, typename std::enable_if<
    !std::is_same<T, V>::value &&
    std::is_same<V, oos::varchar_base>::value &&
    std::is_base_of<V, T>::value
  >::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = from.str();
    success_ = true;
  }

  template < class V >
  void serialize(const char *id, V &/*to*/, typename std::enable_if<
    !std::is_same<T, V>::value &&
    !std::is_base_of<V, T>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    // Todo: throw exception
  }

  template < class V>
  void serialize(const char *id, const identifier<V> &x)
  {
    V val = x.value();
    serialize(id, val);
  }

  void serialize(const char *id, const char *, size_t)
  {
    if (id_ != id) {
      return;
    }
    // convert from string
    // i.e. to_ = to_value<T>(from);
    success_ = true;
  }

private:
  std::string id_;
  T &to_;
  bool success_;
  size_t precision_;
};

/// @cond OOS_DEV

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
  attribute_writer(const std::string &id, char *to, size_t size, size_t precision = 0)
    : id_(id)
    , to_(to)
    , size_(size)
    , success_(false)
    , precision_(precision)
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
  void serialize(const char *id, const V &/*from*/)
  {
    if (id_ != id) {
      return;
    }
    // Todo: convert each type to char*
    success_ = true;
  }

  void serialize(const char*, const date&) {}
  void serialize(const char*, const time&) {}
  void serialize(const char*, const object_container&) {}
  void serialize(const char*, const basic_identifier &) {}

  void serialize(const char *id, const char *from, size_t size)
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
  size_t precision_;
};

template <>
class attribute_writer<std::string>
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
  attribute_writer(const std::string &id, std::string &to, size_t precision = 0)
    : id_(id)
    , to_(to)
    , success_(false)
    , precision_(precision)
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
  void serialize(const char *id, const V &from, typename std::enable_if< std::is_integral<V>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = std::to_string(from);
    success_ = true;
  }

  template < class V >
  void serialize(const char *id, const V &from, typename std::enable_if< std::is_floating_point<V>::value>::type* = 0)
  {
    if (id_ != id) {
      return;
    }
    to_ = oos::to_string(from, precision_);
    success_ = true;
  }

  void serialize(const char *id, const char &from)
  {
    if (id_ != id) {
      return;
    }
    to_ = std::to_string(from);
    success_ = true;
  }

  void serialize(const char *id, const std::string &from)
  {
    if (id_ != id) {
      return;
    }
    to_ = from;
    success_ = true;
  }

  void serialize(const char *id, const oos::varchar_base &from)
  {
    if (id_ != id) {
      return;
    }
    to_ = from.str();
    success_ = true;
  }
  void serialize(const char *id, const date &from)
  {
    if (id_ != id) {
      return;
    }
    to_ = to_string(from);
    success_ = true;
  }
  void serialize(const char *id, const oos::time &from)
  {
    if (id_ != id) {
      return;
    }
    to_ = to_string(from);
    success_ = true;
  }
  void serialize(const char *id, const basic_object_holder &x)
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
  void serialize(const char*, const object_container&) {}
  void serialize(const char *id, const basic_identifier &x)
  {
    if (id_ != id) {
      return;
    }
    std::stringstream to;
    x.print(to);
    to_ = to.str();
    success_ = true;
  }

  void serialize(const char *id, const char *from, size_t size)
  {
    if (id_ != id) {
      return;
    }
    to_.assign(from, size);
    success_ = true;
  }

private:
  std::string id_;
  std::string &to_;
  bool success_;
  size_t precision_;
};

/// @endcond

}

#endif /* ATTRIBUTE_SERIALIZER_HPP */
