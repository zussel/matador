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

#include "tools/convert.hpp"
#include "object/object_convert.hpp"
#include "object/object_ptr.hpp"

#include <stdexcept>
#include <type_traits>

/// @cond OOS_DEV

#ifdef WIN32
#define CPP11_TYPE_TRAITS_NS std::tr1
#else
#define CPP11_TYPE_TRAITS_NS std
#endif

/// @endcond OOS_DEV

namespace oos {

class object_base_ptr;
/*
template < int CP >
void
convert(const long &from, object_ptr<object> &to,
        typename oos::enable_if<CP == convert_strict>::type* = 0)
{
  throw std::bad_cast();
}
template < int CP >
void
convert(const long &, object_ptr<object> &,
        typename oos::enable_if<((CP & convert_fitting_weak) > 0)>::type* = 0)
{
  std::cout << "converting id to object_ptr\n";
}
*/
/**
 * @tparam T The type of the attribute to set.
 * @class attribute_reader
 * @brief Set an attribute value of an object.
 *
 * A given attribute value of a template type is
 * tried to set into an object. Therefor the attribute
 * with given name must be found and the value must
 * be convertible into the objects attribute.
 */
template < class T >
class attribute_reader
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
  
  virtual ~attribute_reader() {}

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
  void read(const char *id, V &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

private:
  std::string id_;
  const T &from_;
  bool success_;

  size_t from_size;
  size_t to_size;
  int precision;
};

/**
 * @tparam T The type of the attribute to retieve.
 * @class attribute_writer
 * @brief Retrieve an attribute value from an object.
 *
 * A attribute value of a template type is
 * tried to retriev from an object. Therefor the attribute
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
   */
  attribute_writer(const std::string &id, T &to, int precision)
    : id_(id)
    , to_(to)
    , success_(false)
    , precision_(precision)
    , size_(0)
  {}
   
  attribute_writer(const std::string &id, T &to, int size, int precision)
    : id_(id)
    , to_(to)
    , success_(false)
    , precision_(precision)
    , size_(size)
  {}
   
  virtual ~attribute_writer() {}

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
  void write(const char *id, const V &from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /*
  template < class V >
  void write(const char *id, const object_ptr<V> &from)
  {
    if (id_ != id) {
      return;
    }
//    convert(from, to_);
    success_ = true;
  }
  */

private:
  std::string id_;
  T &to_;
  bool success_;
  int precision_;
  int size_;
};

}

#endif /* ATTRIBUTE_SERIALIZER_HPP */
