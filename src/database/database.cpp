#include "database/database.hpp"
#include "database/action.hpp"
#include "database/transaction.hpp"

#include "object/object_store.hpp"

#include <iostream>

using namespace std;

namespace oos {

database_impl::database_impl()
{}

database_impl::~database_impl()
{}

void database_impl::visit(insert_action *)
{
}

void database_impl::visit(update_action *)
{
}

void database_impl::visit(delete_action *)
{
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
  if (!transaction_stack_.empty()) {
    cout << "unregister transaction observer [" << transaction_stack_.top()->tro_.get() << "]\n";
    ostore_.unregister_observer(transaction_stack_.top()->tro_.get());
  }
  transaction_stack_.push(tr);
  cout << "register transaction observer [" << tr->tro_.get() << "]\n";
  ostore_.register_observer(tr->tro_.get());
}

void database::pop_transaction()
{
  cout << "unregister transaction observer [" << transaction_stack_.top()->tro_.get() << "]\n";
  ostore_.unregister_observer(transaction_stack_.top()->tro_.get());
  transaction_stack_.pop();
  if (!transaction_stack_.empty()) {
    cout << "register transaction observer [" << transaction_stack_.top()->tro_.get() << "]\n";
    ostore_.register_observer(transaction_stack_.top()->tro_.get());
  }
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

}
