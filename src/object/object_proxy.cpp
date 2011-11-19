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

#include <iostream>

namespace oos {

object_proxy::object_proxy(object_store *os)
  : obj(NULL)
  , id(0)
  , ref_count(0)
  , ptr_count(0)
  , ostore(os)
{}

object_proxy::object_proxy(long i, object_store *os)
  : obj(NULL)
  , id(i)
  , ref_count(0)
  , ptr_count(0)
  , ostore(os)
{}

object_proxy::object_proxy(object *o, object_store *os)
  : obj(o)
  , id((o ? o->id() : 0))
  , ref_count(0)
  , ptr_count(0)
  , ostore(os)
{}

object_proxy::~object_proxy()
{
  if (obj) {
//    std::cout << "deleting proxy [" << this << "] with object [" << obj << "]\n";
    delete obj;
  } else {
//    std::cout << "deleting proxy [" << this << "]\n";
  }
}

void object_proxy::unlink()
{
  if (prev) {
    prev->next = next;
  }
  if (next) {
    next->prev = prev;
  }
  prev.reset();
  next.reset();
}

/*
void object_proxy::insert(object_proxy* oproxy)
{
  // link oproxy before this node
  oproxy->prev = prev;
  oproxy->next = this;
  if (prev) {
    prev->next = oproxy;
  }
  prev = oproxy;
}
*/
void object_proxy::clear()
{
  if (obj) {
    delete obj;
    obj = NULL;
  }
  ostore = NULL;
}

void object_proxy::link_ref()
{
  if (obj) {
//    std::cout << "proxy [" << this << "] called link_ref() value [" << ref_count + 1 << "]\n";
    ++ref_count;
  }
}

void object_proxy::unlink_ref()
{
  if (obj) {
//    std::cout << "proxy [" << this << "] called unlink_ref() value [" << ref_count - 1 << "]\n";
    --ref_count;
  }
}

void object_proxy::link_ptr()
{
  if (obj) {
//    std::cout << "proxy [" << this << "] called link_ptr() value [" << ptr_count + 1 << "]\n";
    ++ptr_count;
  }
}

void object_proxy::unlink_ptr()
{
  if (obj) {
//    std::cout << "proxy [" << this << "] called unlink_ptr() value [" << ptr_count - 1 << "]\n";
    --ptr_count;
  }
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
