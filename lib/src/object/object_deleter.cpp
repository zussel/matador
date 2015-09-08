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
#include "object/serializable.hpp"
#include "object/object_list.hpp"

using namespace std::placeholders;

namespace oos {

object_deleter::t_object_count_struct::t_object_count_struct(object_proxy *oproxy, bool ignr)
  : proxy(oproxy)
  , ref_count(oproxy->ref_count())
  , ptr_count(oproxy->ptr_count())
  , ignore(ignr)
{}

object_deleter::~object_deleter()
{}

bool
object_deleter::is_deletable(object_proxy *proxy)
{
  object_count_map.clear();
  object_count_map.insert(std::make_pair(proxy->id(), t_object_count(proxy, false)));

  // start collecting information
  proxy->obj()->deserialize(*this);
  
  return check_object_count_map();
}

bool object_deleter::is_deletable(object_container &oc)
{
  object_count_map.clear();
  oc.for_each(std::bind(&object_deleter::check_object_list_node, this, _1));
  return check_object_count_map();
}

void object_deleter::read_value(const char*, object_base_ptr &x)
{
  if (!x.ptr()) {
    return;
  }
  check_object(x.proxy_, x.is_reference());
}

void object_deleter::read_value(const char*, object_container &x)
{
  x.for_each(std::bind(&object_deleter::check_object_list_node, this, _1));
}

void object_deleter::read_value(char const *id, primary_key_base &x)
{
  x.deserialize(id, *this);
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

void object_deleter::check_object(object_proxy *proxy, bool is_ref)
{
  std::pair<t_object_count_map::iterator, bool> ret = object_count_map.insert(std::make_pair(proxy->id(), t_object_count(proxy)));
  if (!is_ref) {
    --ret.first->second.ptr_count;
  } else {
    --ret.first->second.ref_count;
  }
  if (!is_ref) {
    ret.first->second.ignore = false;
    proxy->obj()->deserialize(*this);
  }
}

void
object_deleter::check_object_list_node(object_proxy *proxy)
{
  std::pair<t_object_count_map::iterator, bool> ret = object_count_map.insert(std::make_pair(proxy->id(), t_object_count(proxy, false)));
  
  /**********
   * 
   * serializable is already in list and will
   * be ignored on deletion so set
   * ignore flag to false because this
   * node must be deleted
   * 
   **********/
  if (!ret.second && ret.first->second.ignore) {
    ret.first->second.ignore = false;
  }

  // start collecting information
  proxy->obj()->deserialize(*this);
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
    } else if (first->second.ref_count == 0 && first->second.ptr_count == 0) {
      ++first;
    } else {
      return false;
    }
  }
  return true;
}

}
