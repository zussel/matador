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

namespace oos {

object_creator::~object_creator() {}

void object_creator::read_value(const char*, object_base_ptr &x)
{
  // mark serializable pointer as internal
  x.is_internal_ = true;
  if (!x.is_reference()) {
    if (x.ptr()) {
      // do the pointer count
      x.proxy_->link_ptr();
      object_proxy_stack_.push(x.proxy_);
      x.ptr()->deserialize(*this);
      object_proxy_stack_.pop();
    }
  } else if (x.proxy_) {
    // count reference
    x.proxy_->link_ref();
  }
}

void object_creator::read_value(const char*, object_container &x)
{
  // set parent serializable (if available)
  if (!object_proxy_stack_.empty()) {
    x.owner(object_proxy_stack_.top());
  }
  ostore_.insert(x);
}


//void object_creator::read_value(char const *id, basic_identifier &x)
//{
//  x.deserialize(id, *this);
//}

}
