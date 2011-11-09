#include "database/transaction.hpp"
#include "database/database.hpp"
#include "database/action.hpp"

#include <iostream>

using namespace std;

namespace oos {

bool transaction::backup_visitor::backup(action *act)
{
  act->accept(this);
  return true;
}

void transaction::backup_visitor::visit(insert_action *)
{
  // nothing to do
}

void transaction::backup_visitor::visit(update_action *a)
{
  // serialize object
  serializer_.serialize(a->obj(), buffer_);
}

void transaction::backup_visitor::visit(delete_action *a)
{
  // serialize object
  serializer_.serialize(a->obj(), buffer_);
}

bool transaction::restore_visitor::restore(action *act)
{
  act->accept(this);
  return true;
}

void transaction::restore_visitor::visit(insert_action *a)
{
  // remove object from object store
  ostore_.remove_object(a->obj(), false);
}

void transaction::restore_visitor::visit(update_action *a)
{
  // deserialize data from buffer into object
  serializer_.deserialize(a->obj(), buffer_);
}

void transaction::restore_visitor::visit(delete_action *a)
{
  // create object with id and deserialize
  object *o = ostore_.create(a->type().c_str());
  // data from buffer into object
  serializer_.deserialize(o, buffer_);
  // insert object
  object_ptr<object> optr = ostore_.insert(o);
  // ERROR: throw exception if id of object
  //        isn't valid (in use)
}

transaction::transaction_observer::transaction_observer(transaction &tr)
  : tr_(tr)
{
  tr_.db()->ostore().register_observer(this);
}

transaction::transaction_observer::~transaction_observer()
{
  tr_.db()->ostore().unregister_observer(this);
}

void
transaction::transaction_observer::on_insert(object *o)
{
  cout << "inserting " << *o << endl;
  /*****************
   * 
   * backup inserted object
   * on rollback the object
   * is removed from object
   * store
   * 
   *****************/
  tr_.backup(new insert_action(o));
}

void
transaction::transaction_observer::on_update(object *o)
{
  cout << "updating " << *o << endl;
  /*****************
   * 
   * backup updated object
   * on rollback the object
   * is restored to old values
   * 
   *****************/
  tr_.backup(new update_action(o));
}

void
transaction::transaction_observer::on_delete(object *o)
{
  cout << "deleting " << *o << endl;
  /*****************
   * 
   * backup deleted object
   * on rollback the object
   * is restored into the
   * object store
   * 
   *****************/
  tr_.backup(new delete_action(o));
}

transaction::transaction(database *db)
  : db_(db)
  , id_(0)
  , backup_visitor_(object_buffer_)
  , restore_visitor_(object_buffer_, db->ostore())
{}

transaction::~transaction()
{}

long transaction::id_counter = 0;

long
transaction::id() const
{
  return id_;
}

void
transaction::begin()
{
  id_ = ++transaction::id_counter;
  db_->push_transaction(this);
  tro_.reset(new transaction_observer(*this));
  cout << "starting transaction [" << id_ << "]\n";
}

void
transaction::commit()
{
  tro_.reset();
  if (!db_->current_transaction() || db_->current_transaction() != this) {
    // throw db_exception();
    cout << "commit: transaction [" << id_ << "] isn't current transaction\n";
  } else {
    /****************
     * 
     * Pop transaction from stack
     * and execute all actions
     * change state to comitted
     * 
     ****************/
    db_->pop_transaction();
    iterator first = action_list_.begin();
    iterator last = action_list_.end();
    while (first != last) {
      db_->execute_action(*first++);
    }
  }
}

void
transaction::rollback()
{
  tro_.reset();
  if (!db_->current_transaction() || db_->current_transaction() != this) {
    // throw db_exception();
    cout << "rollback: transaction [" << id_ << "] isn't current transaction\n";
  } else {
    /**************
     *
     * rollback transaction
     * restore objects
     * and finally pop transaction
     *
     **************/
    iterator first = action_list_.begin();
    iterator last = action_list_.end();
    while (first != last) {
      action *act = (*first++);
      restore_visitor_.restore(act);
    }
    db_->pop_transaction();
  }
}

database*
transaction::db()
{
  return db_;
}

const database*
transaction::db() const
{
  return db_;
}

void
transaction::backup(action *a)
{
  backup_visitor_.backup(a);
  action_list_.push_back(a);
}

}
