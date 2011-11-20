#include "database/transaction.hpp"
#include "database/database.hpp"
#include "database/action.hpp"

#include <iostream>

using namespace std;

namespace oos {

transaction_impl::transaction_impl(transaction &tr)
  : tr_(tr)
{
//  tr_.db()->ostore().register_observer(this);
}

transaction_impl::~transaction_impl()
{
//  tr_.db()->ostore().unregister_observer(this);
}

void
transaction_impl::on_insert(object *o)
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
transaction_impl::on_update(object *o)
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
transaction_impl::on_delete(object *o)
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

void transaction_impl::rollback()
{
  /**************
   *
   * rollback transaction
   * restore objects
   * and finally pop transaction
   *
   **************/
  while (!tr_.empty()) {
    transaction::iterator i = tr_.begin();
    std::auto_ptr<action> a(*i);
    tr_.erase(i);
    tr_.restore(a.get());
  }
}

void transaction_impl::commit()
{
  /****************
   * 
   * Pop transaction from stack
   * and execute all actions
   * change state to comitted
   * 
   ****************/
  transaction::iterator first = tr_.begin();
  transaction::iterator last = tr_.end();
  while (first != last) {
    delete *first++;
    //tr_.db()->execute_action(*first++);
    //throw exception();
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
  serializer_.serialize(a->obj(), *buffer_);
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
  ostore_->remove_object(a->obj(), false);
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
  object_proxy_ptr oproxy = ostore_->find_proxy(a->id());
  /*
  if (!o) {
    // create object with id and deserialize
    o = ostore_->create(a->type().c_str());
    // data from buffer into object
    serializer_.deserialize(o, *buffer_, ostore_);
    // insert object
    ostore_->insert_object(o, false);
  } else {
    // data from buffer into object
    serializer_.deserialize(o, *buffer_, ostore_);
  }
  */
  // ERROR: throw exception if id of object
  //        isn't valid (in use)
}

transaction::transaction(database *db)
  : db_(db)
  , id_(0)
  , impl_(db->impl_->create_transaction(*this))
{}

transaction::~transaction()
{
  delete impl_;
}

long transaction::id_counter = 0;

long
transaction::id() const
{
  return id_;
}

void
transaction::start()
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
//  tro_.reset(new transaction_observer(*this));
  db_->push_transaction(this);
  cout << "starting transaction [" << id_ << "]\n";
}

void
transaction::commit()
{
//  tro_.reset();
  if (!db_->current_transaction() || db_->current_transaction() != this) {
    // throw db_exception();
    cout << "commit: transaction [" << id_ << "] isn't current transaction\n";
  } else {
    impl_->commit();
    // clear actions
    action_list_.clear();
    object_buffer_.clear();
    id_set_.clear();
    db_->pop_transaction();
  }
}

void
transaction::rollback()
{
//  tro_.reset();
  if (!db_->current_transaction() || db_->current_transaction() != this) {
    // throw db_exception();
    cout << "rollback: transaction [" << id_ << "] isn't current transaction\n";
  } else {
    // call transaction implementations rollback
    impl_->rollback();
    // clear actions
    action_list_.clear();
    object_buffer_.clear();
    id_set_.clear();
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
  /*************
   * 
   * try to find object with
   * id in action map
   * 
   *************/
  
  id_set_t::iterator i = id_set_.find(a->obj()->id());
  if (i == id_set_.end()) {
    cout << "TR (" << id_ << ") couldn't find id [" << a->obj()->id() << "] of object [" << a->obj()->object_type() << "]: inserting action\n";
    backup_visitor_.backup(a, &object_buffer_);
    action_list_.push_back(a);
    id_set_.insert(a->obj()->id());
  } else {
    // find action with id in list
    cout << "TR (" << id_ << ") found id [" << a->obj()->id() << "] of object [" << a->obj()->object_type() << "]\n";
    iterator first = action_list_.begin();
    iterator last = action_list_.end();
    while (first != last) {
      if ((*first)->obj()->id() == *i) {
        break;
      }
      ++first;
    }
    if (first == last) {
      // couldn't find action
      // error, throw exception
      cout << "TR (" << id_ << ") FATAL ERROR: couldn't find corresponding object in action list!\n";
    } else {
      if (a->type() == action::DELETE && (*first)->type() == action::INSERT) {
        cout << "TR (" << id_ << ") new action is delete, old action is insert: removing action\n";
        // within this transaction inserted
        // object is also deleted
        // so we can remove insert and
        // delete action, because object
        // has never been existed
        action_list_.erase(first);
        id_set_.erase(i);
      } else if (a->type() == action::DELETE && (*first)->type() == action::UPDATE) {
        cout << "TR (" << id_ << ") new action is delete, old action is update: replace action\n";
        // updated object is also deleted
        // replace update action with delete
        // action
        *first = a;
      } else {
        cout << "TR (" << id_ << ") nothing todo\n";
        delete a;
      }
    }
  }
}

void transaction::restore(action *a)
{
  restore_visitor_.restore(a, &object_buffer_, &db_->ostore());
}

transaction::iterator transaction::begin()
{
  return action_list_.begin();
}

transaction::const_iterator transaction::begin() const
{
  return action_list_.begin();
}

transaction::iterator transaction::end()
{
  return action_list_.end();
}

transaction::const_iterator transaction::end() const
{
  return action_list_.end();
}

transaction::iterator transaction::erase(transaction::iterator i)
{
  return action_list_.erase(i);
}

bool transaction::empty() const
{
  return action_list_.empty();
}

size_t transaction::size() const
{
  return action_list_.size();
}

}
