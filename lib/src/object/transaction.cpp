//
// Created by sascha on 3/9/16.
//

#include "object/object_proxy.hpp"
#include "object/transaction.hpp"

namespace oos {

transaction::transaction(oos::object_store &store)
  : store_(store)
  , inserter_(actions_)
{}

void transaction::begin()
{

}

void transaction::commit()
{

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
}

void transaction::abort()
{

}

void transaction::backup(const action_ptr &a, const oos::object_proxy *proxy)
{

}

void transaction::restore(const action_ptr &a)
{

}

void transaction::cleanup()
{
  actions_.clear();
  object_buffer_.clear();
  id_map_.clear();
//  db_.pop_transaction();
}


}