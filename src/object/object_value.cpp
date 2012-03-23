/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY {} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "object/object_value.hpp"
#include "object/object.hpp"

namespace oos {

object_value_base::~object_value_base()
{}

void object_value_base::retrieve(const object *o)
{
  o->write_to(this);
}

void object_value_base::update(object *o)
{
  o->read_from(this);
}

void object_value_base::mark_modified(object *o)
{
  o->mark_modified();
}

namespace detail {

void
updater<varchar_base&, varchar_base&>::update(const char *id, const char *f, bool &r, object_value<varchar_base> &ov, object *o, varchar_base &master, varchar_base &slave)
{
  if (strcmp(f, id) != 0) {
    return;
  }
  ov.mark_modified(o);
  slave = master;
  r = true;
}

void
updater<std::string, varchar_base>::update(const char *id, const char *f, bool &r, object_value<std::string> &ov, object *o, const std::string &master, varchar_base &slave)
{
  if (strcmp(f, id) != 0) {
    return;
  }
  ov.mark_modified(o);
  slave = master;
  r = true;
}

}

}
