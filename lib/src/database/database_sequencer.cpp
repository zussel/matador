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
#include "database/database_exception.hpp"
#include "database/query.hpp"
#include "database/statement.hpp"
#include "database/result.hpp"

#include "object/object_atomizer.hpp"

#include "tools/convert.hpp"

namespace oos {

database_sequencer::database_sequencer(database &db)
  : db_(db)
  , backup_(0)
  , sequence_(0)
  , name_("object")
  , update_(0)
{
}

database_sequencer::~database_sequencer()
{
  destroy();
}

void database_sequencer::deserialize(object_reader &r)
{
  r.read("name", name_);
  r.read("number", sequence_);
}

void database_sequencer::serialize(object_writer &w) const
{
  w.write("name", name_);
  w.write("number", sequence_);
}

long database_sequencer::init()
{
  return sequence_;
}

long database_sequencer::reset(long id)
{
  sequence_ = id;
  return sequence_;
}

long database_sequencer::next()
{
  return ++sequence_;
}

long database_sequencer::current() const
{
  return sequence_;
}

long database_sequencer::update(long id)
{
  if (id > sequence_) {
    sequence_ = id;
  }
  return sequence_;
}

void database_sequencer::create()
{
  query q(db_);
  result *res = q.create("oos_sequence", this).execute();
  
  delete res;
  
  res = q.reset().select(this).from("oos_sequence").where("name='object'").execute();

  if (res->fetch()) {
    // get sequence number
    res->get(1, sequence_);
  } else {
    // TODO: check result
    result *res2 = q.reset().insert(this, "oos_sequence").execute();
    delete res2;
  }
  delete res;

  update_ = q.reset().update("oos_sequence", this).where("name='object'").prepare();
}

void database_sequencer::load()
{
  query q(db_);
  result *res = q.reset().select(this).from("oos_sequence").where("name='object'").execute();

  if (res->fetch()) {
    // get sequence number
    res->get(1, sequence_);
  } else {
    delete res;
    throw database_exception("database::sequencer", "couldn't fetch sequence");
  }
  delete res;

  if (!update_) {
    update_ = q.reset().update("oos_sequence", this).where("name='object'").prepare();
  }
}

void database_sequencer::begin()
{
  // backup current sequence id from object store
  backup_ = current();
}

void database_sequencer::commit()
{
  update_->bind(this);
  // TODO: check result
  result *res = update_->execute();
  delete res;
  update_->reset();
}

void database_sequencer::rollback()
{
  reset(backup_);
}

void database_sequencer::drop()
{
  query q(db_);
  // TODO: check result
  result *res = q.drop("oos_sequence").execute();
  delete res;
}

void database_sequencer::destroy()
{
  if (update_) {
    delete update_;
    update_ = 0;
  }
}


}
