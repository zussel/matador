//
// Created by sascha on 3/9/16.
//

#include "object/transaction.hpp"
#include "object/object_store.hpp"

namespace oos {

transaction::transaction(oos::object_store &store)
  : store_(store)
  , inserter_(actions_)
{
}

void transaction::begin()
{
  store_.push_transaction(this);
  // Todo: call database begin
}

void transaction::commit()
{
  cleanup();
  store_.pop_transaction();
}

void transaction::rollback()
{
  if (!store_.current_transaction() || store_.current_transaction() != this) {
    throw object_exception("transaction: transaction isn't current transaction");
  } else {
    /**************
     *
     * rollback transaction
     * restore objects
     * and finally pop transaction
     * clear insert action map
     *
     **************/

    size_t s = actions_.size();
    while (!actions_.empty()) {
      action_iterator i = actions_.begin();
      action_ptr a = *i;
      actions_.erase(i);
      restore(a);
    }

//    db_.rollback();

    // clear container
    cleanup();
  }
  store_.pop_transaction();
}

void transaction::backup(const action_ptr &a, const oos::object_proxy *proxy)
{
  a->backup(object_buffer_);
  action_iterator i = actions_.insert(actions_.end(), a);
  id_map_.insert(std::make_pair(proxy->id(), i));
}

void transaction::restore(const action_ptr &a)
{
  a->restore(object_buffer_, &store_);
}

void transaction::cleanup()
{
  actions_.clear();
  object_buffer_.clear();
  id_map_.clear();
//  db_.pop_transaction();
}


}