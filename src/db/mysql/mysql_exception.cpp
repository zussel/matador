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

#include "matador/db/mysql/mysql_exception.hpp"

#include "matador/sql/database_error.hpp"

#include <string>

namespace matador {

namespace mysql {

void throw_error(MYSQL *db, const std::string &source, const std::string &sql)
{
  throw database_error(mysql_error(db), source, mysql_errno(db), sql);
}

void throw_stmt_error(MYSQL_STMT *stmt, const std::string &source, const std::string &sql)
{
  throw database_error(mysql_stmt_error(stmt), source, mysql_stmt_errno(stmt), sql);
}

}

}
