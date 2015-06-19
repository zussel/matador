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

#include "object/object_proxy.hpp"
#include "object/serializable.hpp"
#include "object/object_store.hpp"

using namespace std;

namespace oos {

primary_key_serializer object_proxy::pk_serializer = primary_key_serializer();

object_proxy::object_proxy(object_store *os)
  : prev_(nullptr)
  , next_(nullptr)
  , obj_(nullptr)
  , oid(0)
  , ref_count_(0)
  , ptr_count_(0)
  , ostore_(os)
  , node_(nullptr)
{}


object_proxy::object_proxy(unsigned long i, object_store *os)
  : prev_(nullptr)
  , next_(nullptr)
  , obj_(nullptr)
  , oid(i)
  , ref_count_(0)
  , ptr_count_(0)
  , ostore_(os)
  , node_(nullptr)
{}

object_proxy::object_proxy(serializable *o, object_store *os)
  : prev_(nullptr)
  , next_(nullptr)
  , obj_(o)
  , oid(0)
  , ref_count_(0)
  , ptr_count_(0)
  , ostore_(os)
  , node_(nullptr)
{
  primary_key_.reset(pk_serializer.serialize(o));
}

object_proxy::object_proxy(serializable *o, unsigned long id, object_store *os)
  : prev_(nullptr)
  , next_(nullptr)
  , obj_(o)
  , oid(id)
  , ref_count_(0)
  , ptr_count_(0)
  , ostore_(os)
  , node_(nullptr)
{
  if (obj_ != nullptr) {
    primary_key_.reset(pk_serializer.serialize(obj_));
  }
}

object_proxy::~object_proxy()
{
  if (ostore_ && id() > 0) {
    ostore_->delete_proxy(id());
  }
  if (obj_) {
    delete obj_;
  }
  ostore_ = 0;
  for (ptr_set_t::iterator i = ptr_set_.begin(); i != ptr_set_.end(); ++i) {
    (*i)->proxy_ = 0;
  }
}
  
serializable *object_proxy::obj()
{
  return obj_;
}

const serializable *object_proxy::obj() const {
  return obj_;
}

object_store *object_proxy::ostore() const
{
  return ostore_;
}

prototype_node *object_proxy::node() const
{
  return node_;
}

void object_proxy::link(object_proxy *successor)
{
  // link serializable proxy before this node_
  prev_ = successor->prev_;
  next_ = successor;
  if (successor->prev_) {
    successor->prev_->next_ = this;
  }
  successor->prev_ = this;
}

void object_proxy::unlink()
{
  if (prev_) {
    prev_->next_ = next_;
  }
  if (next_) {
    next_->prev_ = prev_;
  }
  prev_ = 0;
  next_ = 0;
  node_ = 0;
}

void object_proxy::link_ref()
{
  if (obj_) {
    ++ref_count_;
  }
}

void object_proxy::unlink_ref()
{
  if (obj_) {
    --ref_count_;
  }
}

void object_proxy::link_ptr()
{
  if (obj_) {
    ++ptr_count_;
  }
}

void object_proxy::unlink_ptr()
{
  if (obj_) {
    --ptr_count_;
  }
}

bool object_proxy::linked() const
{
  return node_ != 0;
}

object_proxy *object_proxy::next() const
{
  return next_;
}

object_proxy *object_proxy::prev() const
{
  return prev_;
}

unsigned long object_proxy::ref_count() const
{
  return ref_count_;
}

unsigned long object_proxy::ptr_count() const
{
  return ptr_count_;
}

  void object_proxy::reset(serializable *o)
{
  ref_count_ = 0;
  ptr_count_ = 0;
  obj_ = o;
  oid = 0;
  node_ = 0;
}

void object_proxy::add(object_base_ptr *ptr)
{
  ptr_set_.insert(ptr);
}

bool object_proxy::remove(object_base_ptr *ptr)
{
  return ptr_set_.erase(ptr) == 1;
}

bool object_proxy::valid() const
{
  return ostore_ && node_ && prev_ && next_;
}

unsigned long object_proxy::id() const {
  return oid;
}

void object_proxy::id(unsigned long i)
{
  oid = i;
}

bool object_proxy::has_primary_key() const
{
  return primary_key_ != nullptr;
}

std::shared_ptr<primary_key_base> object_proxy::pk() const
{
  return std::shared_ptr<primary_key_base>();
}

std::ostream& operator <<(std::ostream &os, const object_proxy &op)
{
  os << "proxy [" << &op << "] prev_ [" << op.prev_ << "] next_ [" << op.next_ << "] serializable [" << op.obj_ << "] refs [" << op.ref_count_ << "] ptrs [" << op.ptr_count_ << "]";
  return os;
}

}
