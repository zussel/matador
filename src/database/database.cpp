#include "database/database.hpp"
#include "database/action.hpp"
#include "database/transaction.hpp"

#include "object/object_store.hpp"

namespace oos {

database_impl::database_impl()
{}

database_impl::~database_impl()
{}

void database_impl::visit(insert_action *a)
{
}

void database_impl::visit(update_action *a)
{
}

void database_impl::visit(delete_action *a)
{
}

transaction_impl* database_impl::create_transaction_impl() const
{
  return new transaction_impl;
}

database::database(object_store &ostore, const std::string &/*dbstring*/)
  : ostore_(ostore)
{
  impl_ = new database_impl;
}


database::~database()
{
  delete impl_;
}

void database::open()
{
}

void database::close()
{
}

query_result* database::query(const std::string &)
{
  return NULL;
}

object_store& database::ostore()
{
  return ostore_;
}

const object_store& database::ostore() const
{
  return ostore_;
}

void database::push_transaction(transaction *tr)
{
  transaction_stack_.push(tr);
}

void database::pop_transaction()
{
  transaction_stack_.pop();
}

transaction* database::current_transaction() const
{
  return (transaction_stack_.empty() ? 0 : transaction_stack_.top());
}

void
database::execute_action(action *a)
{
  a->accept(impl_);
}

transaction_impl* database::create_transaction_impl() const
{
  return impl_->create_transaction_impl();
}

}
