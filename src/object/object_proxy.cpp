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

namespace oos {

object_proxy::object_proxy(object *o, object_store *os)
  : obj(o)
  , id((o ? o->id() : 0))
  , ostore(os)
{}
object_proxy::~object_proxy()
{
  if (obj) {
    delete obj;
  }
}

void object_proxy::remove()
{
  if (prev) {
    prev->next = next;
  }
  if (next) {
    next->prev = prev;
  }
  prev = NULL;
  next = NULL;
}

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

void object_proxy::clear()
{
  if (obj) {
    delete obj;
    obj = NULL;
  }
  ostore = NULL;
}

std::ostream& operator <<(std::ostream &os, const object_proxy &op)
{
//  if (op) {
    os << "proxy [" << &op << "] prev [" << op.prev << "] next [" << op.next << "] object [" << op.obj << "]";
/*  } else {
    os << "proxy [0]\n";
  }
*/
  return os;
}

}
