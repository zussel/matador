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

#include "object/object_list.hpp"
#include "object/object.hpp"
#include "object/object_linker.hpp"

#include <iostream>

namespace oos {

object_list_base::object_list_base(object *parent, const std::string &list_ref_name)
  : parent_(parent)
  , list_name_(list_ref_name)
{}

object_list_base::~object_list_base()
{}

void object_list_base::clear()
{
  if (parent_) {
    parent_->mark_modified();
  }
}

void object_list_base::install(object_store *ostore)
{
  ostore_ = ostore;
}

void object_list_base::uninstall()
{
    ostore_ = NULL;
//    parent_ = NULL;
}

void object_list_base::mark_modified(object *o)
{
  o->mark_modified();
}

bool object_list_base::set_reference(object *elem)
{
  object_linker ol(elem, parent_, list_name_);
  elem->read_from(&ol);
  return ol.success();
}

}
