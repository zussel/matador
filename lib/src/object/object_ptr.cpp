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

using namespace std;

namespace oos {

object_base_ptr::object_base_ptr(bool is_ref)
  : is_reference_(is_ref)
{}

object_base_ptr::object_base_ptr(const object_base_ptr &x)
  : proxy_(x.proxy_)
  , is_reference_(x.is_reference_)
  , oid_(x.oid_)
{
  if (proxy_) {
    oid_ = proxy_->id();
    proxy_->add(this);
  }
}

object_base_ptr&
object_base_ptr::operator=(const object_base_ptr &x)
{
  if (this != &x && proxy_ != x.proxy_) {
    reset(x.proxy_, x.is_reference_);
  }
  return *this;
}

object_base_ptr::object_base_ptr(object_proxy *op, bool is_ref)
  : proxy_(op)
  , is_reference_(is_ref)
  , is_internal_(false)
  , oid_(0)
{
  if (proxy_) {
    oid_ = proxy_->id();
    proxy_->add(this);
  }
}

object_base_ptr::object_base_ptr(serializable *o, bool is_ref)
  : proxy_(new object_proxy(o, nullptr))
  , is_reference_(is_ref)
  , is_internal_(false)
  , oid_(0)
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
    if (!proxy_->ostore() && proxy_->ptr_set_.empty()) {
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

void
object_base_ptr::reset(object_proxy *proxy, bool is_ref)
{
  if (proxy == proxy_) {
    return;
  }
  if (proxy_) {
    oid_ = 0;
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
    if (!proxy_->ostore() && proxy_->ptr_set_.empty()) {
      delete proxy_;
    }
  }
  proxy_ = proxy;
  is_reference_ = is_ref;
  if (proxy_) {
    oid_ = proxy_->id();
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

void object_base_ptr::reset(const std::shared_ptr<basic_identifier> &id)
{
  if (proxy_ && !proxy_->pk()->is_same_type(*id)) {
      throw object_exception("identifier types are not equal");
  }
  object_proxy *proxy = new object_proxy(id, nullptr);
  reset(proxy);
}

bool
object_base_ptr::is_loaded() const
{
  return (proxy_ && proxy_->obj());
}

unsigned long
object_base_ptr::id() const
{
  return (proxy_ ? proxy_->id() : oid_);
}

void object_base_ptr::id(unsigned long id)
{
  if (proxy_) {
    throw std::logic_error("proxy already set");
  } else {
    oid_ = id;
  }
}

object_store *object_base_ptr::store() const
{
  return (proxy_ ? proxy_->ostore() : nullptr);
}

serializable * object_base_ptr::ptr()
{
  return proxy_ ? proxy_->obj() : nullptr;
}

const serializable * object_base_ptr::ptr() const
{
  return lookup_object();
}

serializable * object_base_ptr::lookup_object()
{
  if (proxy_ && proxy_->obj()) {
    if (proxy_->ostore()) {
      proxy_->ostore()->mark_modified(proxy_);
    }
    return proxy_->obj();
  } else {
    return nullptr;
  }
}

serializable * object_base_ptr::lookup_object() const
{
  return proxy_ ? proxy_->obj() : nullptr;
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
  return (!proxy_ ? 0 : proxy_->ref_count());
}

unsigned long
object_base_ptr::ptr_count() const
{
  return (!proxy_ ? 0 : proxy_->ptr_count());
}

bool object_base_ptr::has_primary_key() const
{
  return (proxy_ ? proxy_->has_primary_key() : false);
}

std::shared_ptr<basic_identifier> object_base_ptr::primary_key() const
{
  return (proxy_ ? proxy_->pk() : nullptr);
}

std::ostream& operator<<(std::ostream &out, const object_base_ptr &x)
{
  if (x.proxy_) {
    if (x.proxy_->obj()) {
      out << "loaded serializable " << x.proxy_->obj();
    } else {
      out << "unload serializable [" << x.id() << "]";
    }
  } else {
      out << "unknown serializable [" << 0 << "]";
  }
  return out;
}

}
