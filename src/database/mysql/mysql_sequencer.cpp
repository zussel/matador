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

#include "database/mysql/mysql_sequencer.hpp"
#include "database/mysql/mysql_database.hpp"
#include "database/mysql/mysql_result.hpp"

#include "database/row.hpp"

#include <iostream>
#include <cstdlib>

namespace oos {

namespace mysql {

mysql_sequencer::mysql_sequencer(mysql_database *db)
  : db_(db)
  , backup_(0)
  , update_(*db)
{}

mysql_sequencer::~mysql_sequencer()
{}

void mysql_sequencer::create()
{
  /**********
   *
   * Create oos_sequence table. It
   * should have two columns:
   * - name (varchar(32))
   * - sequence (integer, not null)
   *
   **********/
  db_->execute("CREATE TABLE IF NOT EXISTS oos_sequence (name VARCHAR(64), sequence INTEGER NOT NULL);");
  
  mysql_statement stmt(*db_);
//  stmt.prepare("SELECT ~sequence FROM oos_sequence WHERE name='object';", 0, 1);
  stmt.prepare("SELECT sequence FROM oos_sequence WHERE name='object';", 0, 1);

  stmt.prepare_result_column<int>(0);

  //stmt.prepare_param<int>(0);

  stmt.execute();

  if (stmt.fetch()) {
    // prepare result
    int id = 0;
    stmt.column(0, id);
    reset(id);
  } else {
    // no such element, insert one
    db_->execute("INSERT INTO oos_sequence (name, sequence) VALUES ('object', 0);");
  }
  // prepare update statement
  update_.prepare("UPDATE oos_sequence SET sequence=? WHERE name='object';", 0, 1);
}

void mysql_sequencer::begin()
{
  // backup current sequence id from object store
  backup_ = current();
}

void mysql_sequencer::commit()
{
  // write current sequence id of object store to db
  update_.bind(1, (int)current());
  update_.execute();
  update_.fetch();
  update_.reset(true);
  backup_ = 0;
}

void mysql_sequencer::rollback()
{
  // reset backed up sequence into object store
  reset(backup_);
}

void mysql_sequencer::drop()
{
  // drop table (do we need this)
  db_->execute("DROP TABLE oos_sequence;");
}

void mysql_sequencer::destroy()
{
}

}

}
