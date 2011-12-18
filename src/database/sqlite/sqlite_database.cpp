#include "database/sqlite/sqlite_database.hpp"

sqlite_database::sqlite_database()
  : db_(0)
{
  int ret = sqlite3_open();
}

sqlite_database::~sqlite_database()
{
}
