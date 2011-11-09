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

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/object_atomizer.hpp"

namespace oos {

object::object()
	: id_(0)
{
}

object::~object()
{
}

void object::read_from(object_atomizer *a)
{
	a->read_long("id", id_);
}

void object::write_to(object_atomizer *a) const
{
	a->write_long("id", id_);
}

const char* object::object_type() const
{
  return typeid(*this).name();
}

long object::id() const
{
	return id_;
}

void object::id(long oid)
{
	id_ = oid;
}

object_store* object::ostore() const
{
  return proxy_ ? proxy_->ostore : 0;
}

void object::mark_modified()
{
  if (!proxy_ || !proxy_->ostore) {
    // throw exception
    return;
  }
  proxy_->ostore->mark_modified(proxy_);
}

std::ostream& operator <<(std::ostream &os, const object &o)
{
  os << "object " << typeid(o).name() << " (" << &o << ") [" << o.id_ << "]";
  return os;
}

}
