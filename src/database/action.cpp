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

#include <algorithm>

namespace oos
{

insert_action::insert_action(const std::string &t)
  : type_(t)
{}

insert_action::~insert_action()
{}

void insert_action::accept(action_visitor *av)
{
  av->visit(this);
}

std::string insert_action::type() const
{
  return type_;
}

insert_action::iterator insert_action::begin()
{
  return object_list_.begin();
}

insert_action::const_iterator insert_action::begin() const
{
  return object_list_.begin();
}

insert_action::iterator insert_action::end()
{
  return object_list_.end();
}

insert_action::const_iterator insert_action::end() const
{
  return object_list_.end();
}

bool insert_action::empty() const
{
  return object_list_.empty();
}

struct object_by_id : public std::unary_function<long, bool>
{
  object_by_id(long id)
    : id_(id)
  {}
  bool operator()(const object *o) const
  {
    return (o && o->id() == id_);
  }
  
  long id_;
};

insert_action::iterator insert_action::find(long id)
{
  return std::find_if(object_list_.begin(), object_list_.end(), object_by_id(id));
}

insert_action::const_iterator insert_action::find(long id) const
{
  return std::find_if(object_list_.begin(), object_list_.end(), object_by_id(id));
}

void insert_action::push_back(object *o)
{
  object_list_.push_back(o);
}

insert_action::iterator insert_action::erase(insert_action::iterator i)
{
  return object_list_.erase(i);
}

object* update_action::obj()
{
  return obj_;
}

const object* update_action::obj() const
{
  return obj_;
}

delete_action::delete_action(object *o)
  : object_type_(o ? o->object_type() : "")
  , obj_(o)
  , id_(o ? o->id() : 0)
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

object* delete_action::obj()
{
  return obj_;
}

const object* delete_action::obj() const
{
  return obj_;
}

long delete_action::id() const
{
  return id_;
}

}
