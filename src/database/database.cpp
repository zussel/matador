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
#include "database/database_factory.hpp"
#include "database/action.hpp"
#include "database/transaction.hpp"
#include "database/reader.hpp"
#include "database/statement_helper.hpp"
#include "database/statement_binder.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

#include "database/sqlite/sqlite_database.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;

namespace oos {

database::database(object_store &ostore, const std::string &dbstring)
  : ostore_(ostore)
{
  // parse dbstring
  std::string::size_type pos = dbstring.find(':');
  std::string type = dbstring.substr(0, pos);
  std::string db = dbstring.substr(pos + 3);

  // get driver factory singleton
  database_factory &df = database_factory::instance();

  // try to create database implementation
  impl_ = df.create(type, this);

  impl_->open(db);

  // if database is new create all tables
  statement_helper helper;

  prototype_iterator first = ostore_.begin();
  prototype_iterator last = ostore_.end();
  while (first != last) {
      const prototype_node &node = (*first++);
      if (node.abstract) {
        continue;
      }
      std::auto_ptr<object> o(node.producer->create());
      
      // create all tables in database
      std::string sql = helper.create(o.get(), node.type, statement_helper::CREATE);
      
      statement_impl *stmt = impl_->create_statement();
      stmt->prepare(sql);
      // execute create statement
      stmt->step();

      sql = helper.create(o.get(), node.type, statement_helper::SELECT);
      stmt = impl_->create_statement();
      stmt->prepare(sql);
      impl_->store_statement(std::string(o->object_type()) + "_SELECT", database_impl::statement_impl_ptr(stmt));

      sql = helper.create(o.get(), node.type, statement_helper::INSERT);
      stmt = impl_->create_statement();
      stmt->prepare(sql);
      impl_->store_statement(std::string(o->object_type()) + "_INSERT", database_impl::statement_impl_ptr(stmt));

      sql = helper.create(o.get(), node.type, statement_helper::UPDATE, "id=?");
      stmt = impl_->create_statement();
      stmt->prepare(sql);
      impl_->store_statement(std::string(o->object_type()) + "_UPDATE", database_impl::statement_impl_ptr(stmt));

      sql = helper.create(o.get(), node.type, statement_helper::DEL, "id=?");
      stmt = impl_->create_statement();
      stmt->prepare(sql);
      impl_->store_statement(std::string(o->object_type()) + "_DELETE", database_impl::statement_impl_ptr(stmt));
  }
}


database::~database()
{
}

void database::open()
{
}

void database::create()
{
  /*****************
   * 
   * traverse over all prototypes
   * create an object and the create 
   * table statement
   * 
   *****************/
//  statement_helper creator(std::tr1::bind());
  prototype_iterator first = ostore_.begin();
  prototype_iterator last = ostore_.end();
  while (first != last) {
    object *o = (first++)->producer->create();
    /*************************
     * 
     * statement to create
     * CREATE TABLE IF NOT EXISTS
     * o->object_type()
     *  
     *************************/
    delete o;
  }
}

void database::close()
{
}

bool database::load()
{
  prototype_iterator first = ostore_.begin();
  prototype_iterator last = ostore_.end();
  while (first != last) {
    const prototype_node &node = (*first++);
    if (node.abstract) {
      continue;
    }
    // create dummy object
    std::auto_ptr<object> o(node.producer->create());
    reader rdr(*this, o.get());
    while(rdr.read()) {
      std::auto_ptr<object> obj(node.producer->create());
      obj->read_from(&rdr);
      ostore_.insert(obj.release());
    }
  }
  /************
   *
   * init/adjust id counter
   *
   ************/
  //ostore_.sequencer().init();
  return true;
}

result* database::query(const std::string &)
{
  return NULL;
}

object_store& database::ostore()
{
  return ostore_;
}

const object_store& database::ostore() const
{
  return ostore_;
}

void database::push_transaction(transaction *tr)
{
  if (!transaction_stack_.empty()) {
//    cout << "unregister transaction observer [" << transaction_stack_.top() << "]\n";
    ostore_.unregister_observer(transaction_stack_.top());
  }
  transaction_stack_.push(tr);
//  cout << "register transaction observer [" << tr << "]\n";
  ostore_.register_observer(tr);
}

void database::pop_transaction()
{
//  cout << "unregister transaction observer [" << transaction_stack_.top() << "]\n";
  ostore_.unregister_observer(transaction_stack_.top());
  transaction_stack_.pop();
  if (!transaction_stack_.empty()) {
//    cout << "register transaction observer [" << transaction_stack_.top() << "]\n";
    ostore_.register_observer(transaction_stack_.top());
  }
}

object* database::load(const std::string &type, int id)
{
  return NULL;
}

void database::begin(transaction &tr)
{
  push_transaction(&tr);
  impl_->begin();
}

void database::commit(transaction &tr)
{
  // execute begin statement
  statement_binder binder;

  transaction::insert_action_map_t::const_iterator ifirst = tr.insert_action_map_.begin();
  transaction::insert_action_map_t::const_iterator ilast = tr.insert_action_map_.end();
  while (ifirst != ilast) {

    statement_impl_ptr stmt = find_statement(ifirst->first + "_INSERT");
      
    transaction::const_iterator first = ifirst->second.begin();
    transaction::const_iterator last = ifirst->second.end();
    while (first != last) {
      action *a(*first++);
      
      binder.bind(stmt.get(), a->obj(), false);
      
      stmt->step();
      stmt->reset(true);
    }
    ++ifirst;
  }
  transaction::const_iterator first = action_list_.begin();
  transaction::const_iterator last = maction_list_.end();
  while (first != last) {
    (*first++)->accept(impl_);
  }

  // write sequence to db
  sequencer_->commit();



//  impl_->commit(tr.insert_action_map_, tr.action_list_);
}

void database::rollback()
{
  impl_->rollback();
}

statement_impl* database::create_statement_impl() const
{
  return impl_->create_statement();
}

transaction* database::current_transaction() const
{
  return (transaction_stack_.empty() ? 0 : transaction_stack_.top());
}

}
