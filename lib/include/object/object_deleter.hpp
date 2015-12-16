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

#ifndef OBJECT_DELETER_HPP
#define OBJECT_DELETER_HPP

#ifdef _MSC_VER
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
  #define OOS_API
#endif

#include "object/access.hpp"
#include "object/object_proxy.hpp"
#include "object/has_one.hpp"

#include <map>

namespace oos {

class basic_object_holder;
class object_container;
class object_proxy;
class basic_identifier;

/**
 * @cond OOS_DEV
 * @class object_deleter
 * @brief Checks if an serializable could be deleted
 *
 * This class checks wether a given serializable or a
 * given object_list_base and their children objects
 * could be deleted or not.
 * If the check was successful, all the deletable serializable
 * can be accepted via the iterators.
 */
class object_deleter
{
private:
  typedef struct t_object_count_struct
  {
    t_object_count_struct(object_proxy *oproxy, bool ignr = true);

    object_proxy *proxy;
    unsigned long ref_count;
    unsigned long ptr_count;
    bool ignore;
  } t_object_count;

private:
  typedef std::map<unsigned long, t_object_count> t_object_count_map;

public:
  typedef t_object_count_map::iterator iterator;             /**< Shortcut the serializable map iterator */
  typedef t_object_count_map::const_iterator const_iterator; /**< Shortcut the serializable map const_iterator */

  /**
   * Creates an instance of the object_deleter
   */
  object_deleter();

  ~object_deleter();

  /**
   * Checks wether the given serializable is deletable.
   *
   * @param proxy The object_proxy to be checked.
   * @return True if the serializable could be deleted.
   */
  template < class T >
  bool is_deletable(object_proxy *proxy, T *o)
  {
    object_count_map.clear();
    object_count_map.insert(std::make_pair(proxy->id(), t_object_count(proxy, false)));

    // start collecting information
    oos::access::serialize(*this, *o);

    return check_object_count_map();
  }

  /**
   * Checks wether the given object_container is deletable.
   *
   * @param ovector The object_container to be checked.
   * @return True if the object_container could be deleted.
   */
  bool is_deletable(object_container &oc);

  /**
   * @brief Returns the first deletable serializable.
   *
   * If the check was made and was successful this
   * returns the first deletable serializable.
   */
  iterator begin();

  /**
   * @brief Returns the first deletable serializable.
   *
   * If the check was made and was successful this
   * returns the last deletable serializable.
   */
  iterator end();

  template < class T >
  void serialize(const T &x)
  {
    oos::access::serialize(*this, x);
  }

  template < class T >
  void serialize(const char*, const T&) {}

  void serialize(const char*, const char*, size_t) {}
  template < class T >
  void serialize(const char*, const has_one<T> &x, cascade_type cascade)
  {
    if (!x.ptr()) {
      return;
    }
    std::pair<t_object_count_map::iterator, bool> ret = object_count_map.insert(std::make_pair(x.proxy_->id(), t_object_count(x.proxy_)));
    if (cascade | cascade_type::DELETE) {
      --ret.first->second.ptr_count;
    } else {
      --ret.first->second.ref_count;
    }
    if (cascade | cascade_type::DELETE) {
      ret.first->second.ignore = false;
      oos::access::serialize(*this, *x.get());
    }

//    check_object(x, !(cascade | cascade_type::DELETE));
//    check_object(x.proxy_, x.get(), x.is_reference());
  }

  void serialize(const char*, object_container &x);
  template < class T >
  void serialize(const char *id, identifier<T> &x)
  {
    T val = x.value();
    serialize(id, val);
  }

//  template < class T >
//  void check_object(const has_one<T> &x, bool is_ref)
//  {
//    std::pair<t_object_count_map::iterator, bool> ret = object_count_map.insert(std::make_pair(x.proxy_->id(), t_object_count(x.proxy_)));
//    if (!is_ref) {
//      --ret.first->second.ptr_count;
//    } else {
//      --ret.first->second.ref_count;
//    }
//    if (!is_ref) {
//      ret.first->second.ignore = false;
//      oos::access::serialize(*this, x);
//    }
//  }
  void check_object_list_node(object_proxy *proxy);
  bool check_object_count_map() const;

private:
  t_object_count_map object_count_map;
};
/// @endcond
}

#endif /* OBJECT_DELETER_HPP */
