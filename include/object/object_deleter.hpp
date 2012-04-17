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

#include "object/object_atomizer.hpp"

#include <map>

namespace oos {

class object;
class object_base_ptr;

/**
 * @cond OOS_DEV
 * @class object_deleter
 * @brief Checks if an object could be deleted
 *
 * This class checks wether a given object or a
 * given object_list_base and their children objects
 * could be deleted or not.
 * If the check was successful, all the deletable object
 * can be accepted via the iterators.
 */
class object_deleter : public object_atomizer
{
private:
  typedef struct t_object_count_struct
  {
    t_object_count_struct(object *o, bool ignr = true);

    object *obj;
    unsigned long ref_count;
    unsigned long ptr_count;
    bool ignore;
  } t_object_count;

private:
  typedef std::map<unsigned long, t_object_count> t_object_count_map;

public:
  typedef t_object_count_map::iterator iterator;             /**< Shortcut the object map iterator */
  typedef t_object_count_map::const_iterator const_iterator; /**< Shortcut the object map const_iterator */

  /**
   * Creates an instance of the object_deleter
   */
  object_deleter();

  virtual ~object_deleter();

  /**
   * Checks wether the given object is deletable.
   *
   * @param obj The object to be checked.
   * @return True if the object could be deleted.
   */
  bool is_deletable(object *obj);

  /**
   * Checks wether the given object_container is deletable.
   *
   * @param ovector The object_container to be checked.
   * @return True if the object_container could be deleted.
   */
  bool is_deletable(object_container &oc);

  /**
   * @brief Returns the first deletable object.
   *
   * If the check was made and was successful this
   * returns the first deletable object.
   */
  iterator begin();

  /**
   * @brief Returns the first deletable object.
   *
   * If the check was made and was successful this
   * returns the last deletable object.
   */
  iterator end();

private:
  virtual void read_object(const char*, object_base_ptr &x);
  virtual void read_object_container(const char*, object_container &x);

  void check_object(object *o, bool is_ref);
  void check_object_list_node(object *node);
  bool check_object_count_map() const;

private:
  t_object_count_map object_count_map;
};
/// @endcond
}

#endif /* OBJECT_DELETER_HPP */
