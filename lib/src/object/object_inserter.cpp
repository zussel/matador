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

#include <iostream>
#include "object/object_inserter.hpp"
#include "object/object_store.hpp"
#include "object/object_list.hpp"

namespace oos {

object_inserter::object_inserter(object_store &ostore)
  : ostore_(ostore)
{}

object_inserter::~object_inserter() {}

void object_inserter::deserialize(const char*, object_container &)
{
//  if (x.ostore()) {
//    return;
//  }
//  // set parent serializable (if available)
//  if (!object_proxy_stack_.empty()) {
//    x.owner(object_proxy_stack_.top());
//  }
//  x.for_each([this](object_proxy *proxy) {
//    bool new_object = object_proxies_.insert(proxy).second;
//    if (!proxy->obj()) {
//      return;
//    }
//    if (new_object) {
//      object_proxy_stack_.push(proxy);
//      proxy->obj()->deserialize(*this);
//      object_proxy_stack_.pop();
//    }
//  });
//  ostore_.insert(x);
}

}
