#include "database/sqlite/sqlite_database.hpp"

#include "database/transaction.hpp"

#include <exception>
#include <sqlite3.h>

namespace oos {
  
namespace sqlite {
  
sqlite_database::sqlite_database(const std::string &db)
  : db_(0)
{
  int ret = sqlite3_open(db.c_str(), &db_);
  if (ret != SQLITE_OK) {
//    throw std::runtime_error("couldn't open database: " + db);
  }
}

sqlite_database::~sqlite_database()
{
  sqlite3_close(db_);
}

void sqlite_database::visit(insert_action *a)
{
  // create insert statement

}

void sqlite_database::visit(update_action *a)
{
  // create update statement
}

void sqlite_database::visit(delete_action *a)
{
  // create delete statement
}

transaction_impl* sqlite_database::create_transaction(transaction &tr) const
{
  return new transaction_impl(tr);
}

}
}
