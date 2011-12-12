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

#include "object/object_linker.hpp"
#include "object/object.hpp"
#include "object/object_ptr.hpp"

namespace oos {

object_linker::object_linker(object *elem, const object *parent, const std::string &name)
  : elem_(elem)
  , parent_(parent)
  , name_(name)
  , linked_(false)
{}

object_linker::~object_linker() {}

void object_linker::read_object(const char *id, object_base_ptr &x)
{
  if (id == name_) {
    elem_->mark_modified();
    x.reset(parent_);
    linked_ = true;
  }
}

}
