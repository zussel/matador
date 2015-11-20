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

#include "object/serializer.hpp"

#include <map>

namespace oos {

class object_base_ptr;
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
class object_deleter : public generic_deserializer<object_deleter>
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

  virtual ~object_deleter();

  /**
   * Checks wether the given serializable is deletable.
   *
   * @param proxy The object_proxy to be checked.
   * @return True if the serializable could be deleted.
   */
  bool is_deletable(object_proxy *proxy);

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
  void read_value(const char*, const T&) {}

  void read_value(const char*, char*, size_t) {}
  void read_value(const char*, object_base_ptr &x);
  void read_value(const char*, object_container &x);
  void read_value(const char*, basic_identifier &x);

  void check_object(object_proxy *proxy, bool is_ref);
  void check_object_list_node(object_proxy *proxy);
  bool check_object_count_map() const;

private:
  t_object_count_map object_count_map;
};
/// @endcond
}

#endif /* OBJECT_DELETER_HPP */
