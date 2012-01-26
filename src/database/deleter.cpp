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

#include "database/deleter.hpp"
#include "database/database.hpp"
#include "database/statement.hpp"

#include "object/object.hpp"

namespace oos {

deleter::deleter(database &db)
  : db_(db)
{}

deleter::~deleter()
{}

void deleter::remove(object *o)
{
  /******
   * 
   * Find statement in statement
   * info map, bind parameters
   * and execute it.
   *
   ******/
  database::statement_info_map_t::iterator i = db_.statement_info_map_.find(o->object_type());
  if (i == db_.statement_info_map_.end()) {
    // error: couldn't find prepared statements for object
  }
  
  binder_.bind(i->second.remove, o, true);

  i->second.remove->step();
}

}
