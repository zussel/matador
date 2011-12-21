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

#include "object/object_vector.hpp"
#include "object/object.hpp"
#include "object/object_linker.hpp"

#include <iostream>

namespace oos {

object_vector_base::object_vector_base(object *parent,
                                       const std::string &vector_name,
                                       const std::string &index_name)
  : parent_(parent)
  , vector_name_(vector_name)
  , index_name_(index_name)
{}

object_vector_base::~object_vector_base()
{}

void object_vector_base::clear()
{
  if (parent_) {
    parent_->mark_modified();
  }
}

void object_vector_base::install(object_store *ostore)
{
  ostore_ = ostore;
}

void object_vector_base::uninstall()
{
    ostore_ = NULL;
}

void object_vector_base::mark_modified(object *o)
{
  o->mark_modified();
}

bool object_vector_base::set_reference(object *elem)
{
  std::cout << "linking " << *parent_ << " into field " << vector_name_ << " of " << *elem << "\n";
  object_linker ol(elem, parent_, vector_name_);
  elem->read_from(&ol);
  return ol.success();
}

}
