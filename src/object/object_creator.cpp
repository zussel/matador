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

#include "object/object_creator.hpp"
#include "object/object_store.hpp"
#include "object/object_list.hpp"
#include "object/object_vector.hpp"

namespace oos {

object_creator::object_creator(object_store &ostore, bool notify)
  : ostore_(ostore)
  , notify_(notify)
{}

object_creator::~object_creator() {}

void object_creator::read_value(const char*, object_base_ptr &x)
{
  // mark object pointer as internal
  x.is_internal_ = true;
  if (!x.is_reference()) {
    if (!x.ptr()) {
      // create object
      object *o = ostore_.create(x.type());
      o->id(x.id());
      x.reset(ostore_.insert_object(o, notify_));
    } else {
      // do the pointer count
      x.proxy_->link_ptr();
    }
    object_stack_.push(x.ptr());
    x.ptr()->deserialize(*this);
    object_stack_.pop();
  } else if (x.proxy_) {
    // count reference
    x.proxy_->link_ref();
  }
}

void object_creator::read_value(const char*, object_container &x)
{
  // set parent object (if available)
  if (!object_stack_.empty()) {
    x.parent(object_stack_.top());
  }
  ostore_.insert(x);
}

}
