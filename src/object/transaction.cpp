#include "matador/object/transaction.hpp"
#include "matador/object/object_proxy.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/action_remover.hpp"
#include "matador/object/update_action.hpp"
#include "matador/object/delete_action.hpp"

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

unsigned long long transaction::id() const
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
  committing_ = true;
  transaction_data_->observer_->on_commit(transaction_data_->actions_);
  committing_ = false;
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
     * restore_ objects
     * and finally pop transaction
     * clear insert action map
     *
     **************/
    while (!transaction_data_->actions_.empty()) {
      auto i = transaction_data_->actions_.begin();
      action_ptr a = *i;
      transaction_data_->actions_.erase(i);
      restore(a);
    }

    if (committing_) {
      transaction_data_->observer_->on_rollback();
      committing_ = false;
    }

    // clear container
    cleanup();
  }
}

void transaction::on_insert(object_proxy *proxy)
{
  /*****************
   *
   * backup inserted object
   * on rollback the object is removed
   * from object store
   *
   *****************/
  auto i = transaction_data_->id_action_index_map_.find(proxy->id());
  if (i == transaction_data_->id_action_index_map_.end()) {
    // create insert action and insert serializable
    t_action_vector::size_type index = transaction_data_->inserter_.insert(proxy);
    if (index == transaction_data_->actions_.size()) {
      throw_object_exception("transaction: action for object with id " << proxy->id() << " couldn't be inserted");
    } else {
      transaction_data_->id_action_index_map_.insert(std::make_pair(proxy->id(), index));
    }
  } else {
    // ERROR: a serializable with that id already exists
    throw_object_exception("transaction: an object with id " << proxy->id() << " already exists");
  }
}

void transaction::on_update(object_proxy *proxy)
{
  /*****************
   *
   * backup updated serializable
   * on rollback the serializable
   * is restored to old values
   *
   *****************/
  if (transaction_data_->id_action_index_map_.find(proxy->id()) == transaction_data_->id_action_index_map_.end()) {
    auto ua = std::make_shared<update_action>(proxy);
    backup(ua, proxy);
  } else {
    // An object with that id already exists
    // do nothing because the serializable is already
    // backed up
  }
}

void transaction::on_delete(object_proxy *proxy)
{
  /*****************
   *
   * backup deleted serializable
   * on rollback the serializable
   * is restored into the
   * serializable store
   *
   *****************/
  auto i = transaction_data_->id_action_index_map_.find(proxy->id());
  if (i == transaction_data_->id_action_index_map_.end()) {
    backup(std::make_shared<delete_action>(proxy), proxy);
  } else {
    action_remover ar(transaction_data_->actions_);
    ar.remove(i->second, proxy);
  }
}

void transaction::backup(const action_ptr &a, const matador::object_proxy *proxy)
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