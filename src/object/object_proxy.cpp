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
#include "object/object.hpp"
#include "object/object_store.hpp"

#include <iostream>

using namespace std;

namespace oos {

object_proxy::object_proxy(object_store *os)
  : prev(NULL)
  , next(NULL)
  , obj(NULL)
  , id(0)
  , ref_count(0)
  , ptr_count(0)
  , ostore(os)
  , node(0)
{}

object_proxy::object_proxy(long i, object_store *os)
  : prev(NULL)
  , next(NULL)
  , obj(NULL)
  , id(i)
  , ref_count(0)
  , ptr_count(0)
  , ostore(os)
  , node(0)
{}

object_proxy::object_proxy(object *o, object_store *os)
  : prev(NULL)
  , next(NULL)
  , obj(o)
  , id((o ? o->id() : 0))
  , ref_count(0)
  , ptr_count(0)
  , ostore(os)
  , node(0)
{}

object_proxy::~object_proxy()
{
  if (obj) {
    delete obj;
    obj = NULL;
  }
  if (ostore && id > 0) {
    ostore->delete_proxy(id);
  }
  ostore = NULL;
  for (ptr_set_t::iterator i = ptr_set_.begin(); i != ptr_set_.end(); ++i) {
    (*i)->proxy_ = NULL;
  }
}

void object_proxy::link(object_proxy *successor)
{
  // link oproxy before this node
  prev = successor->prev;
  next = successor;
  if (successor->prev) {
    successor->prev->next = this;
  }
  successor->prev = this;
}

void object_proxy::unlink()
{
  if (prev) {
    prev->next = next;
  }
  if (next) {
    next->prev = prev;
  }
  prev = 0;
  next = 0;
  node = 0;
}

void object_proxy::link_ref()
{
  if (obj) {
//    cout << "proxy [" << this << "] called link_ref() value [" << ref_count + 1 << "]\n";
    ++ref_count;
  }
}

void object_proxy::unlink_ref()
{
  if (obj) {
//    cout << "proxy [" << this << "] called unlink_ref() value [" << ref_count - 1 << "]\n";
    --ref_count;
  }
}

void object_proxy::link_ptr()
{
  if (obj) {
//    cout << "proxy [" << this << "] called link_ptr() value [" << ptr_count + 1 << "]\n";
    ++ptr_count;
  }
}

void object_proxy::unlink_ptr()
{
  if (obj) {
//    cout << "proxy [" << this << "] called unlink_ptr() value [" << ptr_count - 1 << "]\n";
    --ptr_count;
  }
}

bool object_proxy::linked() const
{
  return node != 0;
}

void object_proxy::reset(object *o)
{
  ref_count = 0;
  ptr_count = 0;
  id = o->id();
  obj = o;
  node = 0;
}

void object_proxy::add(object_base_ptr *ptr)
{
  ptr_set_.insert(ptr);
}

bool object_proxy::remove(object_base_ptr *ptr)
{
  return ptr_set_.erase(ptr) == 1;
}

std::ostream& operator <<(std::ostream &os, const object_proxy &op)
{
//  if (op) {
    os << "proxy [" << &op << "] prev [" << op.prev << "] next [" << op.next << "] object [" << op.obj << "] refs [" << op.ref_count << "] ptrs [" << op.ptr_count << "]";
/*  } else {
    os << "proxy [0]\n";
  }
*/
  return os;
}

}
