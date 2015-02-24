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

using namespace std;

namespace oos {

object_base_ptr::object_base_ptr(bool is_ref)
  : proxy_(0)
  , is_reference_(is_ref)
  , is_internal_(false)
{}

object_base_ptr::object_base_ptr(const object_base_ptr &x)
  : proxy_(x.proxy_)
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
  : proxy_(op)
  , is_reference_(is_ref)
  , is_internal_(false)
{
  if (proxy_) {
    proxy_->add(this);
  }
}

object_base_ptr::object_base_ptr(object *o, bool is_ref)
  : proxy_(new object_proxy(o, nullptr))
  , is_reference_(is_ref)
  , is_internal_(false)
{
  proxy_->add(this);
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
    /*
     * if proxy was created temporary
     * we can delete it here
     */
    if (!proxy_->ostore) {
      delete proxy_;
    }
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
/*
const char* object_base_ptr::classname() const
{
  if (proxy_ && proxy_->node) {
    return proxy_->node->type.c_str();
  } else {
    return 0;
  }
}
*/
void
object_base_ptr::reset(object_proxy *proxy)
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
  if (proxy) {
    proxy_ = proxy;
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
}

bool
object_base_ptr::is_loaded() const
{
  return (proxy_ && proxy_->obj);
}

unsigned long
object_base_ptr::id() const
{
  return (proxy_ ? proxy_->id() : 0);
}

void object_base_ptr::id(unsigned long id)
{
  if (proxy_) {
    throw std::logic_error("proxy already set");
  } else {
    proxy_ = new object_proxy(id, nullptr);
//    id_ = i;
  }
}


object_store *object_base_ptr::store() const
{
  return (proxy_ ? proxy_->ostore : nullptr);
}

object* object_base_ptr::ptr()
{
  return lookup_object();
}

const object* object_base_ptr::ptr() const
{
  return lookup_object();
}

object* object_base_ptr::lookup_object()
{
  if (proxy_ && proxy_->obj) {
    if (proxy_->ostore) {
      proxy_->ostore->mark_modified(proxy_);
    }
    return proxy_->obj;
  } else {
    return nullptr;
  }
}

object* object_base_ptr::lookup_object() const
{
  if (proxy_ && proxy_->obj) {
    return proxy_->obj;
  } else {
    return nullptr;
  }
}

bool object_base_ptr::is_reference() const
{
  return is_reference_;
}

bool
object_base_ptr::is_internal() const
{
  return is_internal_;
//  return proxy_ && proxy_->ostore;
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

std::ostream& operator<<(std::ostream &out, const object_base_ptr &x)
{
  if (x.proxy_) {
    if (x.proxy_->obj) {
      out << *x.proxy_->obj;
    } else {
      out << "unload object [" << x.proxy_->id() << "]";
    }
  } else {
      out << "unknown object [" << 0 << "]";
  }
  return out;
}

}
