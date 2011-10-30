#include "database/database.hpp"
#include "database/action.hpp"

#include "object/object_store.hpp"

namespace oos {

database::database(object_store &ostore, const std::string &dbstring)
  : ostore_(ostore)
{}

database::~database()
{}

void database::open()
{
}

void database::close()
{
}

query_result* database::query(const std::string &q)
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

}
