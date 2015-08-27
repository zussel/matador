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
  return object_proxy_list_.begin();
}

insert_action::const_iterator insert_action::begin() const
{
  return object_proxy_list_.begin();
}

insert_action::iterator insert_action::end()
{
  return object_proxy_list_.end();
}

insert_action::const_iterator insert_action::end() const
{
  return object_proxy_list_.end();
}

bool insert_action::empty() const
{
  return object_proxy_list_.empty();
}

struct object_by_id : public std::unary_function<unsigned long, bool>
{
  object_by_id(unsigned long id)
    : id_(id)
  {}
  bool operator()(const object_proxy *proxy) const
  {
    return (proxy->obj() && proxy->id() == id_);
  }
  
  unsigned long id_;
};

insert_action::iterator insert_action::find(unsigned long id)
{
  return std::find_if(object_proxy_list_.begin(), object_proxy_list_.end(), object_by_id(id));
}

insert_action::const_iterator insert_action::find(unsigned long id) const
{
  return std::find_if(object_proxy_list_.begin(), object_proxy_list_.end(), object_by_id(id));
}

void insert_action::push_back(object_proxy *proxy)
{
  object_proxy_list_.push_back(proxy);
}

insert_action::iterator insert_action::erase(insert_action::iterator i)
{
  return object_proxy_list_.erase(i);
}

object_proxy* update_action::proxy()
{
  return proxy_;
}

const object_proxy* update_action::proxy() const
{
  return proxy_;
}

delete_action::delete_action(const char *classname, unsigned long id, primary_key_base *pk)
  : classname_(classname)
  , id_(id)
  , pk_(pk)
{}

delete_action::~delete_action()
{}

void delete_action::accept(action_visitor *av)
{
  av->visit(this);
}

const char* delete_action::classname() const
{
  return classname_.c_str();
}

primary_key_base* delete_action::pk() const
{
  return pk_.get();
}

unsigned long delete_action::id() const
{
  return id_;
}

}
