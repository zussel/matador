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
#include <cassert>

using namespace std;

namespace oos {

object_base_ptr::object_base_ptr(bool is_ref)
  : id_(0)
  , is_reference_(is_ref)
  , is_internal_(false)
{}

object_base_ptr::object_base_ptr(const object_base_ptr &x)
  : id_(x.id_)
  , proxy_(x.proxy_)
  , is_reference_(x.is_reference_)
  , is_internal_(false)
{
  if (proxy_) {
    proxy_->add(this);
  }
}

object_base_ptr&
object_base_ptr::operator=(const object_base_ptr &x)
{
  if (this != &x) {
    if (proxy_) {
      if (is_internal_) {
        if (is_reference_) {
          proxy_->unlink_ref();
        } else {
          proxy_->unlink_ptr();
        }
      }
      proxy_->remove(this);
    }
    id_ = x.id_;
    proxy_ = x.proxy_;
    is_reference_ = x.is_reference_;
    if (proxy_) {
      if (is_internal_) {
        if (is_reference_) {
          proxy_->link_ref();
        } else {
          proxy_->link_ptr();
        }
      }
      proxy_->add(this);
    }
  }
  return *this;
}

object_base_ptr::object_base_ptr(object_proxy *op, bool is_ref)
  : id_(op ? op->id : 0)
  , proxy_(op)
  , is_reference_(is_ref)
  , is_internal_(false)
{
  proxy_->add(this);
}

object_base_ptr::object_base_ptr(object *o, bool is_ref)
  : id_(o ? o->id_ : 0)
  , proxy_(o->proxy_)
  , is_reference_(is_ref)
  , is_internal_(false)
{
  if (proxy_) {
    proxy_->add(this);
  }
}

object_base_ptr::~object_base_ptr()
{
  if (proxy_) {
    if (is_internal_) {
      if (is_reference_) {
        proxy_->unlink_ref();
      } else {
        proxy_->unlink_ptr();
      }
    }
    proxy_->remove(this);
  }
}

bool object_base_ptr::operator==(const object_base_ptr &x) const
{
	return x.proxy_ == proxy_;
}

bool object_base_ptr::operator!=(const object_base_ptr &x) const
{
	return !(x == *this);
}

void
object_base_ptr::reset(object *o)
{
  if (proxy_ && is_internal_) {
    if (is_reference_) {
      proxy_->unlink_ref();
    } else {
      proxy_->unlink_ptr();
    }
  }
  if (o) {
    proxy_ = o->proxy_;
    if (proxy_ && is_internal_) {
      if (is_reference_) {
        proxy_->link_ref();
      } else {
        proxy_->link_ptr();
      }
    }
  }
  id_ = (proxy_ ? proxy_->id : 0);
}

bool
object_base_ptr::is_loaded() const
{
    return proxy_ != 0;
}

long
object_base_ptr::id() const
{
    return (proxy_ ? proxy_->id : id_);
}

object*
object_base_ptr::ptr() const
{
    return (proxy_ ? proxy_->obj : NULL);
}

object*
object_base_ptr::lookup_object() const
{
    return (proxy_ ? proxy_->obj : NULL);
}

bool object_base_ptr::is_reference() const
{
  return is_reference_;
}

bool
object_base_ptr::is_internal() const
{
  return is_internal_;
}

unsigned long
object_base_ptr::ref_count() const
{
  return (!proxy_ ? 0 : proxy_->ref_count);
}

unsigned long
object_base_ptr::ptr_count() const
{
  return (!proxy_ ? 0 : proxy_->ptr_count);
}

}
