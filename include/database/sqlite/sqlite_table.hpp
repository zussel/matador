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

#ifndef SQLITE_TABLE_HPP
#define SQLITE_TABLE_HPP

#include "database/table.hpp"

namespace oos {

namespace sqlite {

class sqlite_database;
class sqlite_statement;

class sqlite_table : public table
{
public:
  sqlite_table(sqlite_database &db, const prototype_node &node);

  virtual void prepare();

  virtual database& db();
  virtual const database& db() const;

protected:
  virtual statement* select();
  virtual statement* insert();
  virtual statement* update();
  virtual statement* remove();

private:
  sqlite_database &db_;
  
  sqlite_statement *select_;
  sqlite_statement *insert_;
  sqlite_statement *update_;
  sqlite_statement *delete_;
};

}

}

#endif /* SQLITE_TABLE_HPP */

