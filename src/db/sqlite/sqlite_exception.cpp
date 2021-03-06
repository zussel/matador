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

#include "matador/sql/database_error.hpp"

#include "matador/db/sqlite/sqlite_exception.hpp"

namespace matador {

namespace sqlite {

void throw_database_error(int ec, sqlite3 *db, const std::string &source, const std::string &sql)
{
  if (ec == SQLITE_OK) {
    return;
  }
  throw database_error(sqlite3_errmsg(db), source, sqlite3_errcode(db), sql);
}

}

}
