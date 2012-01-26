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
#include "database/action.hpp"
#include "database/transaction.hpp"
#include "database/reader.hpp"
#include "database/statement_helper.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

#include "database/sqlite/sqlite_database.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;

namespace oos {

database_impl::database_impl()
{}

database_impl::~database_impl()
{}

transaction_impl* database_impl::create_transaction(transaction &tr) const
{
  return new transaction_impl(tr);
}

statement_impl* database_impl::create_statement()
{
  return 0;
}

database::database(object_store &ostore, const std::string &dbstring)
  : destroy_fun_(0)
  , ostore_(ostore)
{
  // parse dbstring
  std::string::size_type pos = dbstring.find(':');
  std::string db_type = dbstring.substr(0, pos);
  std::string db = dbstring.substr(pos + 3);

  // load sqlite library
  // create instance
  if (!db_lib_.load("oos-sqlite")) {
    throw std::runtime_error("couldn't fínd library [" + db_type + "]");
  }
  // get create function
  create_db fun = (create_db)(db_lib_.function("create_database"));

  // create concrete database implementation object
  impl_ = (*fun)(db.c_str());

  // get destroy function
  destroy_fun_ = (destroy_db)db_lib_.function("destroy_database");

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
      
      stmt->step();
      
      statement_info info;
      sql = helper.create(o.get(), node.type, statement_helper::INSERT);

      info.insert = impl_->create_statement();
      info.insert->prepare(sql);

      sql = helper.create(o.get(), node.type, statement_helper::UPDATE, "id=?");

      info.update = impl_->create_statement();
      info.update->prepare(sql);
      sql = helper.create(o.get(), node.type, statement_helper::DEL, "id=?");

      info.remove = impl_->create_statement();
      info.remove->prepare(sql);
      
      statement_info_map_.insert(std::make_pair(o->object_type(), info));
  }
}


database::~database()
{
  if (destroy_fun_) {
  // destroy database object
    (*destroy_fun_)(impl_);
  } else {
    delete impl_;
  }
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
    reader rdr(*this, o.get(), node.type);
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
//    cout << "unregister transaction observer [" << transaction_stack_.top()->transaction_observer_.get() << "]\n";
    ostore_.unregister_observer(&transaction_stack_.top()->observer_);
  }
  transaction_stack_.push(tr);
//  cout << "register transaction observer [" << tr->transaction_observer_.get() << "]\n";
  ostore_.register_observer(&tr->observer_);
}

void database::pop_transaction()
{
//  cout << "unregister transaction observer [" << transaction_stack_.top()->transaction_observer_.get() << "]\n";
  ostore_.unregister_observer(&transaction_stack_.top()->observer_);
  transaction_stack_.pop();
  if (!transaction_stack_.empty()) {
//    cout << "register transaction observer [" << transaction_stack_.top()->transaction_observer_.get() << "]\n";
    ostore_.register_observer(&transaction_stack_.top()->observer_);
  }
}

object* database::load(const std::string &type, int id)
{
  return NULL;
}

statement_impl* database::create_statement_impl() const
{
  return impl_->create_statement();
}

transaction* database::current_transaction() const
{
  return (transaction_stack_.empty() ? 0 : transaction_stack_.top());
}

database::statement_info::statement_info()
  : insert(0)
  , update(0)
  , remove(0)
{}

database::statement_info::~statement_info()
{
  if (insert) {
    delete insert;
  }
  if (update) {
    delete update;
  }
  if (remove) {
    delete remove;
  }
}

}
