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

#include "object/object_deleter.hpp"
#include "object/object.hpp"
#include "object/object_list.hpp"

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

using namespace std::tr1::placeholders;

namespace oos {

object_deleter::t_object_count_struct::t_object_count_struct(object *o, bool ignr)
  : obj(o)
  , ref_count(o->proxy_->ref_count)
  , ptr_count(o->proxy_->ptr_count)
  , ignore(ignr)
{}

object_deleter::object_deleter()
{}

object_deleter::~object_deleter()
{}

bool
object_deleter::is_deletable(object *obj)
{
  object_count_map.clear();
  object_count_map.insert(std::make_pair(obj->id(), t_object_count(obj, false)));

  // start collecting information
  obj->read_from(this);
  
  return check_object_count_map();
}

bool object_deleter::is_deletable(object_list_base &olist)
{
  object_count_map.clear();
  olist.for_each(std::tr1::bind(&object_deleter::check_object_list_node, this, _1));
  return check_object_count_map();
}

void object_deleter::read_object(const char*, base_object_ptr &x)
{
  if (!x.ptr()) {
    return;
  }
  check_object(x.ptr(), x.is_reference());
}

void object_deleter::read_object_list(const char*, object_list_base &x)
{
  x.for_each(std::tr1::bind(&object_deleter::check_object_list_node, this, _1));
}

object_deleter::iterator
object_deleter::begin()
{
  return object_count_map.begin();
}

object_deleter::iterator
object_deleter::end()
{
  return object_count_map.end();
}

void object_deleter::check_object(object *o, bool is_ref)
{
  std::pair<t_object_count_map::iterator, bool> ret = object_count_map.insert(std::make_pair(o->id(), t_object_count(o)));
  if (!is_ref) {
    --ret.first->second.ptr_count;
  } else {
    --ret.first->second.ref_count;
  }
  if (!is_ref) {
    ret.first->second.ignore = false;
    o->read_from(this);
  }
}

void
object_deleter::check_object_list_node(object *node)
{
  std::pair<t_object_count_map::iterator, bool> ret = object_count_map.insert(std::make_pair(node->id(), t_object_count(node, false)));
  
  /**********
   * 
   * object is already in list and will
   * be ignored on deletion so set
   * ignore flag to false because this
   * node must be deleted
   * 
   **********/
  if (!ret.second && ret.first->second.ignore) {
    ret.first->second.ignore = false;
  }

  // start collecting information
  node->read_from(this);
}

bool
object_deleter::check_object_count_map() const
{
  // check the reference and pointer counter of collected objects
  const_iterator first = object_count_map.begin();
  const_iterator last = object_count_map.end();
  while (first != last)
  {
    if (first->second.ignore) {
      ++first;
    } else if (!first->second.ignore && first->second.ref_count == 0 && first->second.ptr_count == 0) {
//    } else if (!first->second.ignore && first->second.obj != obj && first->second.ref_count == 0 && first->second.ptr_count == 0) {
      ++first;
    } else if (first->second.ref_count == 0 && first->second.ptr_count == 0) {
//    } else if (first->second.obj == obj && first->second.ref_count == 0 && first->second.ptr_count == 0) {
//    } else if (first->second.obj == obj && first->second.ref_count == 0 && first->second.ptr_count == 1) {
//    } else if (first->second.ref_count == 0 && first->second.ptr_count == 1) {
      ++first;
    } else {
      return false;
    }
  }
  return true;
}

}
