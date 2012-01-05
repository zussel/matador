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

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

#include <iostream>

using namespace std;

namespace oos {

database_impl::database_impl()
{}

database_impl::~database_impl()
{}

void database_impl::visit(insert_action *)
{
}

void database_impl::visit(update_action *)
{
}

void database_impl::visit(delete_action *)
{
}

transaction_impl* database_impl::create_transaction(transaction &tr) const
{
  return new transaction_impl(tr);
}

database::database(object_store &ostore, const std::string &dbstring)
  : ostore_(ostore)
{
  // parse dbstring
  std::string::size_type pos = dbstring.find(':');
  std::string db = dbstring.substr(0, pos);

  // load sqlite library
  // create instance
  if (!db_lib_.load("oos-" + db + ".dll")) {
    throw std::runtime_error("couldn't fínd library [" + db + "]");
  }
  // get create function
  create_db fun = (create_db)db_lib_.function("create_database");

  // create concrete database implementation object
  impl_ = (*fun)("");

  // get destroy function
  destroy_fun_ = (destroy_db)db_lib_.function("destroy_database");

  /*
  prototype_iterator first = ostore_.begin();
  prototype_iterator last = ostore_.end();
  while (first != last) {
    impl_->prepare(*first++);
  }
  */
}


database::~database()
{
  // destroy database object
  (*destroy_fun_)(impl_);
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
    // create dummy object
    object *o = node.producer->create();
    reader rdr(*this, o, node.type);
    /*
    while(rdr.read()) {
      object *obj = node.producer->create();
      obj->read_from(&rdr);
      ostore_.insert(obj);
      // adjust id counter
    }
    */
    delete o;
  }
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
    ostore_.unregister_observer(transaction_stack_.top()->impl_);
  }
  transaction_stack_.push(tr);
//  cout << "register transaction observer [" << tr->transaction_observer_.get() << "]\n";
  ostore_.register_observer(tr->impl_);
}

void database::pop_transaction()
{
//  cout << "unregister transaction observer [" << transaction_stack_.top()->transaction_observer_.get() << "]\n";
  ostore_.unregister_observer(transaction_stack_.top()->impl_);
  transaction_stack_.pop();
  if (!transaction_stack_.empty()) {
//    cout << "register transaction observer [" << transaction_stack_.top()->transaction_observer_.get() << "]\n";
    ostore_.register_observer(transaction_stack_.top()->impl_);
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

}
