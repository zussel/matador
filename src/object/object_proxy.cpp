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

#include "matador/object/object_store.hpp"

using namespace std;

namespace matador {

object_proxy::object_proxy(const std::shared_ptr<basic_identifier> &pk)
  : primary_key_(pk)
{}

object_proxy::~object_proxy()
{
  if (ostore_ && id() > 0) {
    // Todo: callback to object store?
//    ostore_->delete_proxy(id());
  }
  if (obj_) {
    deleter_(obj_);
  }
  ostore_ = nullptr;
  for (auto &ptr : ptr_set_) {
    ptr->proxy_ = nullptr;
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
  prev_ = nullptr;
  next_ = nullptr;
  node_ = nullptr;
}

unsigned long object_proxy::operator++()
{
//  std::cout << "increasing ref count for " << *this << " new count " << reference_counter_ + 1 << "\n";
  return ++reference_counter_;
}

unsigned long object_proxy::operator++(int)
{
//  std::cout << "increasing ref count for " << *this << " new count " << reference_counter_ + 1 << "\n";
  return reference_counter_++;
}

unsigned long object_proxy::operator--()
{
//  std::cout << "decreasing ref count for " << *this << " new count " << reference_counter_ - 1 << "\n";
  return --reference_counter_;
}

unsigned long object_proxy::operator--(int)
{
//  std::cout << "decreasing ref count for " << *this << " new count " << reference_counter_ - 1 << "\n";
  return reference_counter_--;
}

bool object_proxy::linked() const
{
  return node_ != nullptr;
}

object_proxy *object_proxy::next() const
{
  return next_;
}

object_proxy *object_proxy::prev() const
{
  return prev_;
}

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
  os << "proxy [" << &op << "] (oid: " << op.oid << ", type: " << (op.node_ ? op.node_->type() : op.classname()) << ")"
     << " prev_ [" << op.prev_ << "]"
     << " next_ [" << op.next_ << "] object [" << op.obj_ << "] "
     << " refs [" << op.reference_counter_ << "]";
  return os;
}

}
