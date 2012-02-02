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

#include "database/sqlite/sqlite_sequencer.hpp"
#include "database/sqlite/sqlite_database.hpp"

namespace oos {

namespace sqlite {

sqlite_sequencer::sqlite_sequencer(sqlite_database *db)
  : db_(db)
  , backup_(0)
  , update_(*db)
{}

sqlite_sequencer::~sqlite_sequencer()
{}

void sqlite_sequencer::create()
{
  /**********
   *
   * Create oos_sequence table. It
   * should have two columns:
   * - name (varchar(32))
   * - sequence (integer, not null)
   *
   **********/
  sqlite_statement stmt(*db_);
  stmt.prepare("CREATE TABLE IF NOT EXISTS oos_sequence (name VARCHAR(64), sequence INTEGER NOT NULL);");
  stmt.step();

  stmt.prepare("SELECT sequence FROM oos_sequence WHERE name='object';");
  if (stmt.step()) {
    // element exists
    int id = stmt.column_int(0);
    reset(id);
  } else {
    // no such element, insert one
    stmt.prepare("INSERT INTO oos_sequence (name, sequence) VALUES ('object', 0);");
    stmt.step();
    reset(0);
  }
  // prepare update statement
  update_.prepare("UPDATE oos_sequence SET sequence=? WHERE name='object';");
}

void sqlite_sequencer::begin()
{
  // backup current sequence id from object store
  backup_ = current();
}

void sqlite_sequencer::commit()
{
  // write current sequence id of object store to db
  update_.bind(0, current());
  update_.step();
  update_.reset();
  backup_ = 0;
}

void sqlite_sequencer::rollback()
{
  // reset backed up sequence into object store
  reset(backup_);
}

void sqlite_sequencer::drop()
{
  // drop table (do we need this)
}

}

}
