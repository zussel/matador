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
  : generic_object_reader<object_inserter>(this)
  , ostore_(ostore)
{}

object_inserter::~object_inserter() {}

void object_inserter::insert(object_proxy *proxy)
{
  object_proxies_.clear();
  while (!object_proxy_stack_.empty()) {
    object_proxy_stack_.pop();
  }

  object_proxies_.insert(proxy);
  object_proxy_stack_.push(proxy);
  if (proxy->obj()) {
    proxy->obj()->deserialize(*this);
  }

}

void object_inserter::read_value(const char*, object_base_ptr &x)
{
  // mark serializable pointer as internal
  x.is_internal_ = true;

  if (!x.proxy_) {
    return;
  }

  if (x.is_reference()) {
    x.proxy_->link_ref();
  } else if (x.ptr() && x.id()){
    x.proxy_->link_ptr();
  }
  if (x.ptr()) {
    bool new_object = object_proxies_.insert(x.proxy_).second;
    if (x.id()) {
      // do the pointer count
//      x.proxy_->link_ptr();
      if (new_object) {
        object_proxy_stack_.push(x.proxy_);
        x.ptr()->deserialize(*this);
        object_proxy_stack_.pop();
      }
    } else if (new_object){
      // new object
      ostore_.insert_proxy(x.proxy_);
    }
  }
}

void object_inserter::read_value(const char*, object_container &x)
{
  if (x.ostore()) {
    return;
  }
  // set parent serializable (if available)
  if (!object_proxy_stack_.empty()) {
    x.owner(object_proxy_stack_.top());
  }
  x.for_each([this](object_proxy *proxy) {
    bool new_object = object_proxies_.insert(proxy).second;
    if (!proxy->obj()) {
      return;
    }
    if (new_object) {
      object_proxy_stack_.push(proxy);
      proxy->obj()->deserialize(*this);
      object_proxy_stack_.pop();
    }
  });
  ostore_.insert(x);
}

}
