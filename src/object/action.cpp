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

#include "matador/object/action.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/object_serializer.hpp"

namespace matador
{

action::action()
  : serializer_(new object_serializer)
{}

action::~action()
{
  delete serializer_;
}

void action::remove_proxy(object_proxy *proxy, object_store *store)
{
  store->remove_proxy(proxy);
}

object_proxy* action::find_proxy(object_store *store, unsigned long id)
{
  return store->find_proxy(id);
}

void action::insert_proxy(object_store *store, object_proxy *proxy)
{
  store->insert_proxy(proxy);
}

}
