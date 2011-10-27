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

base_object_ptr::base_object_ptr(bool is_ref)
  : id_(0)
  , is_reference_(is_ref)
  , is_internal_(false)
{}

base_object_ptr::base_object_ptr(const base_object_ptr &x)
  : id_(x.id_)
  , proxy_(x.proxy_)
  , is_reference_(x.is_reference_)
  , is_internal_(false)
//  , is_internal_(x.is_internal_)
{
//  cout << "base object ptr COPY\n";
  /*
  if (proxy_ && is_internal_) {
    if (is_reference_) {
      proxy_->link_ref();
    } else {
      proxy_->link_ptr();
    }
  }
  */
}

base_object_ptr&
base_object_ptr::operator=(const base_object_ptr &x)
{
//  cout << "base object ptr ASSIGN [" << x.type() << "]\n";
  if (this != &x) {
    if (proxy_ && is_internal_) {
      if (is_reference_) {
        proxy_->unlink_ref();
      } else {
        proxy_->unlink_ptr();
      }
    }
//    is_internal_ = false;
    id_ = x.id_;
    proxy_ = x.proxy_;
    is_reference_ = x.is_reference_;
    if (proxy_ && is_internal_) {
      if (is_reference_) {
        proxy_->link_ref();
      } else {
        proxy_->link_ptr();
      }
    }
  }
  return *this;
}

base_object_ptr::base_object_ptr(const object_proxy_ptr &op, bool is_ref)
  : id_(op ? op->id : 0)
  , proxy_(op)
  , is_reference_(is_ref)
  , is_internal_(false)
{
//  cout << "base object ptr CONSTRUCTOR [proxy=" << op.get() << ", is_ref=" << is_ref << "]\n";
}

base_object_ptr::base_object_ptr(object *o, bool is_ref)
  : id_(o ? o->id_ : 0)
  , proxy_(o->proxy_)
  , is_reference_(is_ref)
  , is_internal_(false)
{
//  cout << "base object ptr CONSTRUCTOR [obj=" << o << ", is_ref=" << is_ref << "]\n";
}

base_object_ptr::~base_object_ptr()
{
//  cout << "base object ptr DESTRUCTOR\n";
  if (proxy_ && is_internal_) {
    if (is_reference_) {
      proxy_->unlink_ref();
    } else {
      proxy_->unlink_ptr();
    }
  }
}

bool base_object_ptr::operator==(const base_object_ptr &x) const
{
	return x.proxy_ == proxy_;
}

bool base_object_ptr::operator!=(const base_object_ptr &x) const
{
	return !(x == *this);
}

void
base_object_ptr::reset(object *o)
{
//  cout << "base object ptr RESET [" << type() << "]\n";
  if (proxy_ && is_internal_) {
    if (is_reference_) {
      proxy_->unlink_ref();
    } else {
      proxy_->unlink_ptr();
    }
  }
//  is_internal_ = false;
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
base_object_ptr::is_loaded() const
{
    return proxy_ != 0;
}

long
base_object_ptr::id() const
{
    return (proxy_ ? proxy_->id : id_);
}

object*
base_object_ptr::ptr() const
{
    return (proxy_ ? proxy_->obj : NULL);
}

object*
base_object_ptr::lookup_object() const
{
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

bool
base_object_ptr::is_internal() const
{
  return is_internal_;
}

unsigned long
base_object_ptr::ref_count() const
{
  return (!proxy_ ? 0 : proxy_->ref_count);
}

unsigned long
base_object_ptr::ptr_count() const
{
  return (!proxy_ ? 0 : proxy_->ptr_count);
}

}
