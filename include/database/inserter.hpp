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

#ifndef INSERTER_HPP
#define INSERTER_HPP

#include "database/statement_binder.hpp"

namespace oos {

class database;
class object;

class inserter
{
public:
  inserter(database &db);
  ~inserter();

  /**
   * Insert the given object into
   * the database.
   * 
   * @param o The object to insert.
   */
  void insert(object *o);

private:
  database &db_;
  
  statement_binder binder_;
};

}

#endif /* INSERTER_HPP */
