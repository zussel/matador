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

#include "object/generic_access.hpp"

namespace oos {


sequence::sequence()
{

}

sequence::~sequence()
{

}

void sequence::deserialize(object_reader &r)
{
  r.read("name", name_);
  r.read("number", sequence_);
}

void sequence::serialize(object_writer &w) const
{
  w.write("name", name_);
  w.write("number", sequence_);
}

unsigned long sequence::seq() const
{
  return sequence_;
}

void sequence::seq(unsigned long sequence)
{
  sequence_ = sequence;
}

const varchar<64> &sequence::name() const
{
  return name_;
}

void sequence::name(const varchar<64> &name)
{
  name_ = name;
}

unsigned long sequence::operator++() {
  return ++sequence_;
}

database_sequencer::database_sequencer(database &db)
  : db_(db)
  , backup_(0)
  , update_()
{}

database_sequencer::~database_sequencer()
{
  destroy();
}

unsigned long database_sequencer::init()
{
  return sequence_.seq();
}

unsigned long database_sequencer::reset(unsigned long id)
{
  sequence_.seq(id);
  return sequence_.seq();
}

unsigned long database_sequencer::next()
{
  return ++sequence_;
}

unsigned long database_sequencer::current() const
{
  return sequence_.seq();
}

unsigned long database_sequencer::update(unsigned long id)
{
  if (id > sequence_.seq()) {
    sequence_.seq(id);
  }
  return sequence_.seq();
}

void database_sequencer::create()
{
  query<sequence> q(db_);
  result<sequence> res = q.create("oos_sequence", &sequence_).execute();

  res = q.reset().select(new object_producer<sequence>).from("oos_sequence").where("name='serializable'").execute();

  result_iterator<sequence> first = res.begin();

  if (first != res.end()) {
    unsigned long seq = 0;
    oos::get(first.get(), "number", seq);
    sequence_.seq(seq);
  } else {
    // TODO: check result
    sequence_.name("serializable");
    sequence_.seq(0);
    q.reset().insert(&sequence_, "oos_sequence").execute();
  }

  update_ = q.reset().update(&sequence_, "oos_sequence").where("name='serializable'").prepare();
}

void database_sequencer::load()
{
  query<sequence> q(db_);
  auto res = q.select(new object_producer<sequence>).from("oos_sequence").where("name='serializable'").execute();

  auto first = res.begin();

  if (first != res.end()) {
    unsigned long seq = 0;
    oos::get(first.get(), "number", seq);
    sequence_.seq(seq);
  } else {
    throw database_exception("database::sequencer", "couldn't fetch sequence");
  }

//  if (!update_) {
//    update_.reset(q.reset().update("oos_sequence", &sequence_).where("name='serializable'").prepare());
//  }
  update_ = q.reset().update(&sequence_, "oos_sequence").where("name='serializable'").prepare();
}

void database_sequencer::begin()
{
  // backup current sequence id from serializable store
  backup_ = current();
}

void database_sequencer::commit()
{
  update_.bind(&sequence_);
  // TODO: check result
  update_.execute();
//  update_.clear();
  update_.reset();
}

void database_sequencer::rollback()
{
  reset(backup_);
}

void database_sequencer::drop()
{
  update_.clear();
  query<sequence> q(db_);
  // TODO: check result
  q.drop("oos_sequence").execute();
}

void database_sequencer::destroy()
{
  update_.clear();
}


}
