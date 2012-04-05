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

#include "database/sqlite/sqlite_producer.hpp"
#include "database/sqlite/sqlite_database.hpp"

namespace oos {

namespace sqlite {

sqlite_producer::sqlite_producer()
{}

database* sqlite_producer::create(session *db) const
{
  return new sqlite_database(db);
}

}

}

const oos::sqlite::sqlite_producer producer;

extern "C"
{
  OOS_SQLITE_API const oos::database_producer* get_producer()
  {
    return &producer;
  }
}
