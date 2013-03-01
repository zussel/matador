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

#ifndef MYSQL_TABLE_HPP
#define MYSQL_TABLE_HPP

#include "database/table.hpp"

namespace oos {

namespace mysql {

class mysql_database;
class mysql_statement;

class mysql_table : public table
{
public:
  mysql_table(mysql_database &db, const prototype_node &node);
  virtual ~mysql_table();

  virtual void prepare();

  virtual database& db();
  virtual const database& db() const;

protected:
  virtual statement* select();
  virtual statement* insert();
  virtual statement* update();
  virtual statement* remove();

private:
  mysql_database &db_;

  mysql_statement *select_;
  mysql_statement *insert_;
  mysql_statement *update_;
  mysql_statement *delete_;
};

}

}

#endif /* MYSQL_TABLE_HPP */
