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
#include "database/database_exception.hpp"
#include "database/database_sequencer.hpp"
#include "database/transaction.hpp"
#include "database/statement.hpp"
#include "database/table.hpp"
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

void database::open(const std::string &connection)
{
  if (is_open()) {
    return;
  } else {
    // open the database backend
    on_open(connection);
    
    // create all tables
    prototype_iterator first = db_->ostore().begin();
    prototype_iterator last = db_->ostore().end();
    while (first != last) {
      if (!first->abstract) {
        table_map_.insert(std::make_pair(first->type, table_ptr(new table(*this, *first))));
      }
      ++first;
    }

    // setup sequencer
    sequencer_backup_ = db_->ostore().exchange_sequencer(sequencer_);
  }
}

void database::close()
{
  if (!is_open()) {
    return;
  } else {
    if (sequencer_backup_) {
      db()->ostore().exchange_sequencer(sequencer_backup_);
    }
    sequencer_->destroy();
    
    table_map_.clear();
    
    // close database backend
    on_close();
  }
}

void database::create()
{
  // create sequencer
  sequencer_->create();

  table_map_t::iterator first = table_map_.begin();
  table_map_t::iterator last = table_map_.end();
  while (first != last) {
    ((first++)->second->create());
  }
}

void database::create(const prototype_node &node)
{
  table_map_t::iterator i = table_map_.find(node.type);
  if (i == table_map_.end()) {
    // create table
    table_ptr tbl(new table(*this, node));
    
    i = table_map_.insert(std::make_pair(node.type, tbl)).first;
  }
  i->second->create();
}

serializable * database::insert(object_proxy *proxy)
{
  table_map_t::iterator i = table_map_.find(proxy->node()->type);
  if (i == table_map_.end()) {
    throw database_exception("db::insert", "unknown type");
  } else {
    i->second->insert(proxy->obj());
  }
  return proxy->obj();
}

serializable * database::update(object_proxy *proxy)
{
  table_map_t::iterator i = table_map_.find(proxy->node()->type);
  if (i == table_map_.end()) {
    throw database_exception("db::update", "unknown type");
  } else {
    i->second->update(proxy->obj());
  }
  return proxy->obj();
}

void database::load(const prototype_node &node)
{
  table_map_t::iterator i = table_map_.find(node.type);
  if (i == table_map_.end()) {
    // create table
    table_ptr tbl(new table(*this, node));
    
    i = table_map_.insert(std::make_pair(node.type, tbl)).first;
  }
  
  i->second->load(db_->ostore());
}

bool database::is_loaded(const std::string &name) const
{
#ifdef _MSC_VER
  table_map_t::const_iterator i = table_map_.find(name);
  if (i == table_map_.end()) {
    throw std::out_of_range("unknown key");
  } else {
    return i->second->is_loaded();
  }
#else
  return table_map_.at(name)->is_loaded();
#endif
}

result database::execute(std::string sql, std::shared_ptr<object_base_producer> ptr)
{
  return result(on_execute(sql, ptr), this);
}

statement database::prepare(const oos::sql &sql, std::shared_ptr<object_base_producer> ptr)
{
  return statement(on_prepare(sql, ptr));
}

void database::drop()
{
  table_map_t::iterator first = table_map_.begin();
  table_map_t::iterator last = table_map_.end();
  while (first != last) {
    ((first++)->second->drop());
  }
  sequencer_->drop();
}

void database::drop(const prototype_node &node)
{
  table_map_t::iterator i = table_map_.find(node.type);
  if (i == table_map_.end()) {
    // create table
    throw database_exception("db", "table not found");
  }
  i->second->drop();
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

database::database_sequencer_ptr database::seq() const
{
  return sequencer_;
}

void database::visit(insert_action *a)
{
  table_map_t::iterator i = table_map_.find(a->type());
  if (i == table_map_.end()) {
    /*
     * TODO: add prototype node to insert action
     * to create the table
     */
    // create table
    //table_ptr tbl(new table(*this, node));
    //i = table_map_.insert(std::make_pair(node.type, tbl)).first;
    throw database_exception("db", "table not found");
  }
  
  
  insert_action::const_iterator first = a->begin();
  insert_action::const_iterator last = a->end();
  while (first != last) {
    object_proxy *proxy = (*first++);

    table_ptr t = i->second;
    t->insert(proxy->obj());
  }
}

void database::visit(update_action *a)
{
  table_map_t::iterator i = table_map_.find(a->proxy()->node()->type);
  if (i == table_map_.end()) {
    throw database_exception("db", "table not found");
  }

  i->second->update(a->proxy()->obj());
}

void database::visit(delete_action *a)
{
  table_map_t::iterator i = table_map_.find(a->classname());
  if (i == table_map_.end()) {
    throw database_exception("db", "table not found");
  }

  // Todo: handle remove action (one solution: store primary key)
//  i->second->remove(a->id());

}

const session* database::db() const
{
  return db_;
}

session* database::db()
{
  return db_;
}

result database::create_result(detail::result_impl *impl) {
  return oos::result(impl, this);
}

}
