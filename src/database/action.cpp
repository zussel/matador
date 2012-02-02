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

#include "database/action.hpp"
#include "object/object.hpp"

namespace oos
{

action::action(action::action_type t, object *o)
  : type_(t)
  , obj_(o)
  , id_((o ? o->id() : 0))
{}

action::~action()
{}

action::action_type action::type() const
{
  return type_;
}

object* action::obj()
{
  return obj_;
}

const object* action::obj() const
{
  return obj_;
}

long action::id() const
{
  return id_;
}

delete_action::delete_action(object *o)
  : action(action::DEL, o)
  , object_type_(o ? o->object_type() : "")
{}

delete_action::~delete_action()
{}

void delete_action::accept(action_visitor *av)
{
  av->visit(this);
}

std::string delete_action::object_type() const
{
  return object_type_;
}

}
