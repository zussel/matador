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

#ifndef UPDATER_HPP
#define UPDATER_HPP

#include "database/statement_binder.hpp"

namespace oos {

class database;
class object;

/// @cond OOS_DEV
class updater
{
public:
  updater(database &db);
  virtual ~updater();

  /**
   * Update the given object.
   * 
   * @param o The object to update
   */
  void update(object *o);

private:
  database &db_;
  
  statement_binder binder_;
};
/// @endcond

}

#endif /* UPDATER_HPP */
