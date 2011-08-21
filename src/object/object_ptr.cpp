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

#include "object/object_ptr.hpp"
#include "object/object_store.hpp"
#include "object/object.hpp"

#include <iostream>

namespace oos {

base_object_ptr::base_object_ptr(bool is_ref)
  : id_(0)
  , proxy_(0)
  , is_reference_(is_ref)
{}

base_object_ptr::base_object_ptr(const base_object_ptr &x)
  : id_(x.id_)
  , proxy_(x.proxy_)
  , is_reference_(x.is_reference_)
{}

base_object_ptr&
base_object_ptr::operator=(const base_object_ptr &x) {
  id_ = x.id_;
  proxy_ = x.proxy_;
  is_reference_ = x.is_reference_;
  return *this;
}

base_object_ptr::base_object_ptr(object_proxy *op, bool is_ref)
  : id_(op->id)
  , proxy_(op)
  , is_reference_(is_ref)
{}

base_object_ptr::base_object_ptr(object *o, bool is_ref)
  : id_(o ? o->id_ : 0)
  , proxy_(o ? o->proxy_ : 0)
  , is_reference_(is_ref)
{}

base_object_ptr::~base_object_ptr()
{}

bool base_object_ptr::operator==(const base_object_ptr &x) const {
	return x.proxy_ == proxy_;
}

bool base_object_ptr::operator!=(const base_object_ptr &x) const {
	return !(x == *this);
}

void
base_object_ptr::reset(object *o) {
  proxy_ = (o ? o->proxy_ : 0);
  id_ = (proxy_ ? proxy_->id : 0);
}

bool
base_object_ptr::is_loaded() const {
    return proxy_ != 0;
}

long
base_object_ptr::id() const {
    return (proxy_ ? proxy_->id : id_);
}

object*
base_object_ptr::ptr() const {
    return (proxy_ ? proxy_->obj : NULL);
}

object*
base_object_ptr::lookup_object() {
    return (proxy_ ? proxy_->obj : NULL);
}

bool base_object_ptr::delete_object()
{
  if (!proxy_) {
    return false;
  }
  return true;
}

bool base_object_ptr::is_reference() const
{
  return is_reference_;
}

}
