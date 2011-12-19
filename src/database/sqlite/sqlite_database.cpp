#include "database/sqlite/sqlite_database.hpp"

#include "database/transaction.hpp"

namespace oos {
  
namespace sqlite {
  
sqlite_database::sqlite_database(const std::string &db)
  : db_(0)
{
//  int ret = sqlite3_open(;
}

sqlite_database::~sqlite_database()
{
}

void sqlite_database::visit(insert_action *a)
{
}

void sqlite_database::visit(update_action *a)
{
}

void sqlite_database::visit(delete_action *a)
{
}

transaction_impl* sqlite_database::create_transaction(transaction &tr) const
{
  return new transaction_impl(tr);
}

}
}
