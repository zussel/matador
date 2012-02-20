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

#include "database/transaction.hpp"
#include "database/database.hpp"
#include "database/database_impl.hpp"
#include "database/inserter.hpp"

#include "tools/byte_buffer.hpp"

#include "object/object_store.hpp"
#include "object/object.hpp"

#include <iostream>

using namespace std;

namespace oos {

class action_inserter : public action_visitor
{
public:
  virtual ~action_inserter() {}

  virtual void visit(create_action*) {}
  virtual void visit(insert_action *a);
  virtual void visit(update_action *a);
  virtual void visit(delete_action *a);
  virtual void visit(drop_action*a) {}
};

class action_remover : public action_visitor
{
public:
  action_remover(transaction::action_list_t &action_list)
    : action_list_(action_list)
    , id_(0)
  {}
  virtual ~action_remover() {}

  bool remove(transaction::action_list_t::iterator i, long id);

  virtual void visit(create_action*) {}
  virtual void visit(insert_action *a);
  virtual void visit(update_action *a);
  virtual void visit(delete_action *a);
  virtual void visit(drop_action*) {}

private:
  transaction::action_list_t &action_list_;
  long id_;
};

void transaction::on_insert(object *o)
{
  
  
  /*****************
   * 
   * try to find action with 
   *
   *****************/
//  cout << "inserting " << *o << endl;
  /*****************
   * 
   * backup inserted object
   * on rollback the object
   * is removed from object
   * store
   * 
   *****************/
  if (id_map_.find(o->id()) == id_map_.end()) {
    // find or create insert action and insert object
    
  } else {
    // ERROR: an object with that id already exists
  }

  if (!has_id(o->id())) {
//    backup(new insert_action(o));
  } else {
    // ERROR: an object with that id already exists
  }
}

void transaction::on_update(object *o)
{
//  cout << "updating " << *o << endl;
  /*****************
   * 
   * backup updated object
   * on rollback the object
   * is restored to old values
   * 
   *****************/
  if (id_map_.find(o->id()) == id_map_.end()) {
    backup(new update_action(o));
  } else {
    // An object with that id already exists
    // do nothing because the object is already
    // backed up
  }

  /*
  if (!has_id(o->id())) {
    backup(new update_action(o));
  } else {
    // An object with that id already exists
    // do nothing because the object is already
    // backed up
  }
  */
}

void transaction::on_delete(object *o)
{
//  cout << "deleting " << *o << endl;
  /*****************
   * 
   * backup deleted object
   * on rollback the object
   * is restored into the
   * object store
   * 
   *****************/

  id_iterator_map_t::iterator i = id_map_.find(o->id());
  if (i == id_map_.end()) {
    backup(new delete_action(o));
  } else {
    action_remover ar(action_list_);
    ar.remove(i->second, o->id());
  }
}

bool transaction::backup_visitor::backup(action *act, byte_buffer *buffer)
{
  buffer_ = buffer;
  act->accept(this);
  buffer_ = NULL;
  return true;
}

void transaction::backup_visitor::visit(insert_action *)
{
  // nothing to do
}

void transaction::backup_visitor::visit(update_action *a)
{
  // serialize object
  serializer_.serialize(a->obj(), *buffer_);
}

void transaction::backup_visitor::visit(delete_action *a)
{
  // serialize object
  //serializer_.serialize(a->obj(), *buffer_);
}

bool transaction::restore_visitor::restore(action *act, byte_buffer *buffer, object_store *ostore)
{
  ostore_ = ostore;
  buffer_ = buffer;
  act->accept(this);
  buffer_ = NULL;
  ostore_ = NULL;
  return true;
}

void transaction::restore_visitor::visit(insert_action *a)
{
  // remove object from object store
  for (insert_action::iterator i = a->begin(); i != a->end(); ++i) {
    ostore_->remove_object(*i, false);
  }
//  ostore_->remove_object(a->obj(), false);
}

void transaction::restore_visitor::visit(update_action *a)
{
  // deserialize data from buffer into object
  serializer_.deserialize(a->obj(), *buffer_, ostore_);
}

void transaction::restore_visitor::visit(delete_action *a)
{
  // check if there is an object with id in
  // object store
  object_proxy *oproxy = ostore_->find_proxy(a->id());
  if (!oproxy) {
    // create proxy
    oproxy = ostore_->create_proxy(a->id());
  }
  if (!oproxy->obj) {
    // create object with id and deserialize
    oproxy->obj = ostore_->create(a->object_type().c_str());
    // data from buffer into object
    serializer_.deserialize(oproxy->obj, *buffer_, ostore_);
    // insert object
    ostore_->insert_object(oproxy->obj, false);
  } else {
    // data from buffer into object
    serializer_.deserialize(oproxy->obj, *buffer_, ostore_);
  }
}

transaction::transaction(database &db)
  : db_(db)
  , id_(0)
{}

transaction::~transaction()
{
}

long transaction::id_counter = 0;

long
transaction::id() const
{
  return id_;
}

void
transaction::begin()
{
  /**************
   * 
   * On begin transaction gets unique
   * id. Transaction is pushed to dbs
   * transactions stack. Previous transactions
   * observer is beeing unregistered and
   * this transactions observer is regsitered.
   *
   **************/
  id_ = ++transaction::id_counter;
  db_.begin(*this);
  cout << "starting transaction [" << id_ << "]\n";
}

void
transaction::commit()
{
  if (!db_.current_transaction() || db_.current_transaction() != this) {
    // throw db_exception();
    cout << "commit: transaction [" << id_ << "] isn't current transaction (" << db_.current_transaction() << ")\n";
  } else {
    // commit all transaction actions
    db_.commit(*this);
    // clear actions
    cleanup();
  }
}

void
transaction::rollback()
{
  if (!db_.current_transaction() || db_.current_transaction() != this) {
    // throw db_exception();
    cout << "rollback: transaction [" << id_ << "] isn't current transaction\n";
  } else {
    /**************
     *
     * rollback transaction
     * restore objects
     * and finally pop transaction
     * clear insert action map
     *
     **************/

    insert_action_map_t::iterator first = insert_action_map_.begin();
    insert_action_map_t::iterator last = insert_action_map_.end();
    while (first != last) {
      while (!first->second.empty()) {
        std::auto_ptr<action> a(first->second.front());
        restore(a.get());
        first->second.pop_front();
      }
      ++first;
    }
    insert_action_map_.clear();
    while (!action_list_.empty()) {
      iterator i = action_list_.begin();
      std::auto_ptr<action> a(*i);
      action_list_.erase(i);
      restore(a.get());
    }

    db_.rollback();

    // clear container
    cleanup();
  }
}

database&
transaction::db()
{
  return db_;
}

const database&
transaction::db() const
{
  return db_;
}

void
transaction::backup(action *a)
{
  /*************
   * 
   * try to find object with
   * id in action map
   * 
   *************/
  backup_visitor_.backup(a, &object_buffer_);
  action_list_.push_back(a);
  id_set_.insert(a->obj()->id());
}

void transaction::restore(action *a)
{
  restore_visitor_.restore(a, &object_buffer_, &db_.ostore());
}

bool transaction::has_id(long id) const
{
  return id_set_.find(id) != id_set_.end();
}

void transaction::cleanup()
{
  insert_action_map_t::iterator first = insert_action_map_.begin();
  insert_action_map_t::iterator last = insert_action_map_.end();
  while (first != last) {
    while (!first->second.empty()) {
      action *a(first->second.front());
      delete a;
      first->second.pop_front();
    }
    ++first;
  }
  insert_action_map_.clear();
  while (!action_list_.empty()) {
    action *a = action_list_.front();
    delete a;
    action_list_.pop_front();
  }

  object_buffer_.clear();
  id_set_.clear();
  db_.pop_transaction();
}

bool action_remover::remove(transaction::action_list_t::iterator i, long id)
{
  id_ = id;
  i->accept(this);
  id_ = 0;
}

void action_remover::visit(insert_action *a)
{
  insert_action::iterator i = a->find(id_);
  if (i != a->end()) {
    a->erase(i);
  }
}

void action_remover::visit(update_action *a)
{
  if (a->obj()->id() == id_) {
    //action_list_
  }
}

void action_remover::visit(delete_action *a)
{
  if (a->id() == id_) {
    // ERROR: object was deleted twice
    // throw exception
  }
}

}
