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

#ifndef SQLITE_STATEMENT_HPP
#define SQLITE_STATEMENT_HPP

#include "database/statement.hpp"

#include <string>

struct sqlite3_stmt;

namespace oos {

namespace sqlite {

class sqlite_database;

class sqlite_statement : public statement_impl
{
public:
  sqlite_statement(sqlite_database &db);
  virtual ~sqlite_statement();

  virtual result* execute(const std::string &sql);
  virtual row* step();

  int prepare(const std::string &sql);
  int finalize();
  int reset();

  int column_count() const;

  const char* column_text(int i) const;
  int column_int(int i) const;
  double column_double(int i) const;

  int column_type(int i) const;

  int bind(int i, double value);
  int bind(int i, int value);
  int bind(int i, const std::string &value);

private:
  sqlite3_stmt *stmt_;
  sqlite_database &db_;
};

}

}

#endif /* SQLITE_STATEMENT_HPP */
