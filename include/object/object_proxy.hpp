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

#ifndef OBJECT_PROXY_HPP
#define OBJECT_PROXY_HPP

#include <ostream>

#ifdef WIN32
#include <memory>
#else
#include <tr1/memory>
#endif

namespace oos {

class object;
class object_store;
class prototype_node;

struct object_proxy;
typedef std::tr1::shared_ptr<object_proxy> object_proxy_ptr;

struct object_proxy {
  explicit object_proxy(object_store *os);
  object_proxy(long i, object_store *os);
  object_proxy(object *o, object_store *os);
  ~object_proxy();
  
  friend std::ostream& operator <<(std::ostream &os, const object_proxy &op);

  void unlink();
  void clear();

  void link_ref();
  void unlink_ref();

  void link_ptr();
  void unlink_ptr();

  bool linked() const;

  void reset(object *o);

  object_proxy_ptr prev;
  object_proxy_ptr next;

  object *obj;
  unsigned long id;
  
  unsigned long ref_count;
  unsigned long ptr_count;

  object_store *ostore;
  prototype_node *node;
};

}

#endif /* OBJECT_PROXY_HPP */
