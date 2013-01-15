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

#include "database/session.hpp"
#include "database/database.hpp"
#include "database/database_sequencer.hpp"
#include "database/transaction.hpp"
#include "database/statement.hpp"
#include "database/result.hpp"

#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

#include <stdexcept>

namespace oos {

database::database(session *db, database_sequencer *seq)
  : db_(db)
  , commiting_(false)
  , sequencer_(seq)
{
}

database::~database()
{}

void database::open(const std::string&)
{
  prototype_iterator first = db_->ostore().begin();
  prototype_iterator last = db_->ostore().end();
  while (first != last) {
    if (!first->abstract) {
      table_info_map_.insert(std::make_pair(first->type, table_info_t(first.get())));
    }
    ++first;
  }

  sequencer_->create();
  // setup sequencer
  sequencer_backup_ = db_->ostore().exchange_sequencer(sequencer_);
}

void database::close()
{
  if (sequencer_backup_) {
    db()->ostore().exchange_sequencer(sequencer_backup_);
  }
  sequencer_->destroy();
  
  statement_impl_map_.clear();
  table_info_map_.clear();
}

void database::load(const prototype_node &node)
{
#ifdef WIN32
  table_info_map_t::iterator i = table_info_map_.find(node.type);
  if (i == table_info_map_.end()) {
    throw std::out_of_range("unknown key");
  } else {
    i->second.is_loaded = true;
  }
#else
  table_info_map_.at(node.type).is_loaded = true;
#endif
}

bool database::is_loaded(const std::string &name) const
{
#ifdef WIN32
  table_info_map_t::const_iterator i = table_info_map_.find(name);
  if (i == table_info_map_.end()) {
    throw std::out_of_range("unknown key");
  } else {
    return i->second.is_loaded;
  }
#else
  return table_info_map_.at(name).is_loaded;
#endif
}

void database::drop()
{
  sequencer_->drop();
}

bool database::store_statement(const std::string &id, database::statement_impl_ptr stmt)
{
  return statement_impl_map_.insert(std::make_pair(id, stmt)).second;
}

database::statement_impl_ptr database::find_statement(const std::string &id) const
{
  statement_impl_map_t::const_iterator i = statement_impl_map_.find(id);
  if (i != statement_impl_map_.end()) {
    return i->second;
  } else {
    return statement_impl_ptr();
  }
}

void database::prepare()
{
  sequencer_->begin();
}

void database::begin()
{
  on_begin();
  commiting_ = true;
}

void database::commit()
{
  // write sequence to db
  sequencer_->commit();

  on_commit();

  commiting_ = false;
}

void database::rollback()
{
  sequencer_->rollback();

  if (commiting_) {
    on_rollback();
    commiting_ = false;
  }
}

const session* database::db() const
{
  return db_;
}

session* database::db()
{
  return db_;
}

}
