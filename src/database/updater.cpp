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

#include "database/updater.hpp"
#include "database/database.hpp"
#include "database/database_impl.hpp"
#include "database/statement.hpp"

#include "object/object.hpp"

namespace oos {

updater::updater(database &db)
  : db_(db)
{}

updater::~updater()
{}

void updater::update(object *o)
{
  /******
   * 
   * Find statement in statement
   * info map, bind parameters
   * and execute it.
   *
   ******/
  database_impl::statement_impl_ptr stmt = db_.impl_->find_statement(std::string(o->object_type()) + "_UPDATE");
  if (!stmt) {
  }
  
  binder_.bind(stmt.get(), o, true);

  stmt->step();
}

}
