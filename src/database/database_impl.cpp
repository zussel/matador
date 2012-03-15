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

#include "database/database.hpp"
#include "database/database_impl.hpp"
#include "database/database_sequencer.hpp"
#include "database/transaction.hpp"
#include "database/statement.hpp"
#include "database/result.hpp"

#include "object/object_store.hpp"

namespace oos {

database_impl::database_impl(database *db, database_sequencer *seq)
  : db_(db)
  , sequencer_(seq)
{}

database_impl::~database_impl()
{}

void database_impl::open(const std::string &db)
{
  sequencer_->create();
  // setup sequencer
  sequencer_backup_ = db_->ostore().exchange_sequencer(sequencer_);
}

void database_impl::close()
{
  if (sequencer_backup_) {
    db()->ostore().exchange_sequencer(sequencer_backup_);
  }
  sequencer_->destroy();
  
  statement_impl_map_.clear();
}

bool database_impl::store_statement(const std::string &id, database_impl::statement_impl_ptr stmt)
{
  return statement_impl_map_.insert(std::make_pair(id, stmt)).second;
}

database_impl::statement_impl_ptr database_impl::find_statement(const std::string &id) const
{
  statement_impl_map_t::const_iterator i = statement_impl_map_.find(id);
  if (i != statement_impl_map_.end()) {
    return i->second;
  } else {
    return statement_impl_ptr();
  }
}

void database_impl::prepare()
{
  sequencer_->begin();
}

void database_impl::begin()
{
  on_begin();
}

void database_impl::commit()
{
  // write sequence to db
  sequencer_->commit();

  on_commit();
}

void database_impl::rollback()
{
  sequencer_->rollback();

  on_rollback();
}

const database* database_impl::db() const
{
  return db_;
}

database* database_impl::db()
{
  return db_;
}

}
