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
  : store_(store)
  , inserter_(actions_)
  , observer_(obsvr)
{}

void transaction::begin()
{
  store_.get().push_transaction(this);
  observer_->on_begin();
}

void transaction::commit()
{
  observer_->on_commit(actions_);
  cleanup();
}

void transaction::rollback()
{
  if (!store_.get().current_transaction() || store_.get().current_transaction() != this) {
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
    while (!actions_.empty()) {
      action_iterator i = actions_.begin();
      action_ptr a = *i;
      actions_.erase(i);
      restore(a);
    }

    observer_->on_rollback();

    // clear container
    cleanup();
  }
}

void transaction::backup(const action_ptr &a, const oos::object_proxy *proxy)
{
  a->backup(object_buffer_);
  actions_.push_back(a);
  id_action_index_map_.insert(std::make_pair(proxy->id(), actions_.size() - 1));
}

void transaction::restore(const action_ptr &a)
{
  a->restore(object_buffer_, &store_.get());
}

void transaction::cleanup()
{
  actions_.clear();
  object_buffer_.clear();
  id_action_index_map_.clear();
  store_.get().pop_transaction();
}


}