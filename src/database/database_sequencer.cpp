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

#include "database/database_sequencer.hpp"
#include "database/query.hpp"
#include "database/statement.hpp"

#include "object/object_atomizer.hpp"

namespace oos {

database_sequencer::database_sequencer(database &db)
  : db_(db)
  , backup_(0)
  , update_(0)
{}

database_sequencer::~database_sequencer()
{
  if (update_) {
    delete update_;
  }
}

void database_sequencer::deserialize(object_reader &r)
{
  r.read("name", name_);
  r.read("sequence", backup_);
}

void database_sequencer::serialize(object_writer &w) const
{
  w.write("name", name_);
  w.write("sequence", backup_);
}

void database_sequencer::create()
{
  query q(db_);
  
}

void database_sequencer::load()
{
}

void database_sequencer::begin()
{
}

void database_sequencer::commit()
{
}

void database_sequencer::rollback()
{
}

void database_sequencer::drop()
{
}

void database_sequencer::destroy()
{
}


}
