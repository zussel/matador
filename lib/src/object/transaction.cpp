//
// Created by sascha on 3/9/16.
//

#include "object/transaction.hpp"
#include "object/object_store.hpp"

namespace oos {

transaction::transaction(oos::object_store &store)
  : transaction(store, std::make_shared<null_observer>())
{}

transaction::transaction(object_store &store, const std::shared_ptr<observer> &obsvr)
  : transaction_data_(std::make_shared<transaction_data>(store, obsvr))
{}

void transaction::begin()
{
  transaction_data_->store_.get().push_transaction(*this);
  transaction_data_->observer_->on_begin();
}

void transaction::commit()
{
  transaction_data_->observer_->on_commit(transaction_data_->actions_);
  cleanup();
}

void transaction::rollback()
{
  if (!transaction_data_->store_.get().has_transaction() ||
      &transaction_data_->store_.get().current_transaction() != this)
  {
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
    while (!transaction_data_->actions_.empty()) {
      action_iterator i = transaction_data_->actions_.begin();
      action_ptr a = *i;
      transaction_data_->actions_.erase(i);
      restore(a);
    }

    transaction_data_->observer_->on_rollback();

    // clear container
    cleanup();
  }
}

void transaction::backup(const action_ptr &a, const oos::object_proxy *proxy)
{
  a->backup(transaction_data_->object_buffer_);
  transaction_data_->actions_.push_back(a);
  transaction_data_->id_action_index_map_.insert(std::make_pair(proxy->id(), transaction_data_->actions_.size() - 1));
}

void transaction::restore(const action_ptr &a)
{
  a->restore(transaction_data_->object_buffer_, &transaction_data_->store_.get());
}

void transaction::cleanup()
{
  transaction_data_->actions_.clear();
  transaction_data_->object_buffer_.clear();
  transaction_data_->id_action_index_map_.clear();
  transaction_data_->store_.get().pop_transaction();
}


}