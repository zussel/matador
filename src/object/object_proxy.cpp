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

#include "oos/object/object_store.hpp"

using namespace std;

namespace oos {

object_proxy::object_proxy() {}

object_proxy::object_proxy(const std::shared_ptr<basic_identifier> &pk)
  : primary_key_(pk)
{}

//object_proxy::object_proxy(unsigned long i)
//  : oid(i)
//{}

object_proxy::~object_proxy()
{
  if (ostore_ && id() > 0) {
    // Todo: callback to object store?
//    ostore_->delete_proxy(id());
  }
  if (obj_) {
    deleter_(obj_);
  }
  ostore_ = 0;
  for (ptr_set_t::iterator i = ptr_set_.begin(); i != ptr_set_.end(); ++i) {
    (*i)->proxy_ = 0;
  }
}

const char *object_proxy::classname() const
{
  return namer_();
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

//void object_proxy::link_ref()
//{
//  if (obj_) {
//    ++ref_count_;
//  }
//}
//
//void object_proxy::unlink_ref()
//{
//  if (obj_) {
//    --ref_count_;
//  }
//}
//
//void object_proxy::link_ptr()
//{
//  if (obj_) {
//    ++ptr_count_;
//  }
//}
//
//void object_proxy::unlink_ptr()
//{
//  if (obj_) {
//    --ptr_count_;
//  }
//}

unsigned long object_proxy::operator++()
{
  return ++reference_counter_;
}

unsigned long object_proxy::operator++(int)
{
  return reference_counter_++;
}

unsigned long object_proxy::operator--()
{
  return --reference_counter_;
}

unsigned long object_proxy::operator--(int)
{
  return reference_counter_--;
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

//unsigned long object_proxy::ref_count() const
//{
//  return ref_count_;
//}
//
//unsigned long object_proxy::ptr_count() const
//{
//  return ptr_count_;
//}

unsigned long object_proxy::reference_count() const
{
  return reference_counter_;
}

void object_proxy::add(object_holder *ptr)
{
  ptr_set_.insert(ptr);
}

bool object_proxy::remove(object_holder *ptr)
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

bool object_proxy::has_identifier() const
{
  return primary_key_ != nullptr;
}

std::shared_ptr<basic_identifier> object_proxy::pk() const
{
  return primary_key_;
}

transaction object_proxy::current_transaction()
{
  return ostore_->current_transaction();
}

bool object_proxy::has_transaction() const
{
  return ostore_->has_transaction();
}

std::ostream& operator <<(std::ostream &os, const object_proxy &op)
{
  os << "proxy [" << &op << "] prev_ [" << op.prev_ << "] next_ [" << op.next_ << "] object [" << op.obj_ << "]";// refs [" << op.ref_count_ << "] ptrs [" << op.ptr_count_ << "]";
  return os;
}

}
