//
// Created by sascha on 3/9/16.
//

#include "matador/object/transaction.hpp"
#include "matador/object/object_store.hpp"

namespace matador {

sequencer transaction::sequencer_ = sequencer();

void transaction::null_observer::on_commit(transaction::t_action_vector &actions)
{
  for (transaction::action_ptr &actptr : actions) {
    actptr->accept(this);
  }
}

void transaction::null_observer::visit(delete_action *act)
{
  act->mark_deleted();
}

transaction::transaction(matador::object_store &store)
  : transaction(store, std::make_shared<null_observer>())
{}

transaction::transaction(object_store &store, const std::shared_ptr<observer> &obsvr)
  : transaction_data_(std::make_shared<transaction_data>(store, obsvr))
{}

transaction::transaction(const transaction &x)
  : transaction_data_(x.transaction_data_)
{}

unsigned long transaction::id() const
{
  return transaction_data_->id_;
}

bool operator==(const transaction &a, const transaction &b)
{
  return a.id() == b.id();
}

bool operator!=(const transaction &a, const transaction &b)
{
  return !(a==b);
}

void transaction::begin()
{
  transaction_data_->store_.get().push_transaction(*this);
}

void transaction::commit()
{
  commiting_ = true;
  transaction_data_->observer_->on_commit(transaction_data_->actions_);
  commiting_ = false;
  cleanup();
}

void transaction::rollback()
{
  if (!transaction_data_->store_.get().has_transaction() ||
      transaction_data_->store_.get().current_transaction() != *this)
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

    if (commiting_) {
      transaction_data_->observer_->on_rollback();
      commiting_ = false;
    }

    // clear container
    cleanup();
  }
}

void transaction::backup(const action_ptr &a, const matador::object_proxy *proxy)
{
  a->backup(transaction_data_->object_buffer_);
  transaction_data_->actions_.push_back(a);
  transaction_data_->id_action_index_map_.insert(std::make_pair(proxy->id(), transaction_data_->actions_.size() - 1));
  std::cout << "transaction: backup proxy " << proxy << "(id: " << proxy->id() << ")\n";
  std::cout << "transaction: id action map size " << transaction_data_->id_action_index_map_.size() << "\n";
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