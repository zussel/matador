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

#include "object/object_container.hpp"

namespace oos {
object_container::object_container()
{}

object_container::~object_container()
{}

object_store* object_container::ostore() const
{
  return ostore_;
}

void object_container::mark_modified(object *o)
{
  //o->mark_modified();
}

void object_container::install(object_store *ostore)
{
  ostore_ = ostore;
}

void object_container::uninstall()
{
  ostore_ = 0;
}

}
