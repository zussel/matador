//
// Created by sascha on 3/9/16.
//

#include "object/object_proxy.hpp"
#include "object/transaction.hpp"

namespace oos {

transaction::transaction(oos::object_store &store)
  : store_(store)
  , inserter_(actions_)
{
}

void transaction::begin()
{
  store_.transaction_ = this;
  // Todo: call database begin
}

void transaction::commit()
{
  cleanup();
  store_.transaction_ = nullptr;
}

void transaction::rollback()
{
//  if (!db_.current_transaction() || db_.current_transaction() != this) {
//    throw database_exception("transaction", "transaction isn't current transaction");
//  } else {
    /**************
     *
     * rollback transaction
     * restore objects
     * and finally pop transaction
     * clear insert action map
     *
     **************/

    while (!actions_.empty()) {
      action_iterator i = actions_.begin();
      action_ptr a = *i;
      actions_.erase(i);
      restore(a);
    }

//    db_.rollback();

    // clear container
    cleanup();
//  }
  store_.transaction_ = nullptr;
}

void transaction::backup(const action_ptr &a, const oos::object_proxy *proxy)
{
  a->backup(object_buffer_);
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