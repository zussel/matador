/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "database/sqlite/sqlite_database.hpp"
#include "database/sqlite/sqlite_statement.hpp"
#include "database/sqlite/sqlite_result.hpp"

#include "database/database.hpp"
#include "database/transaction.hpp"
#include "database/statement_binder.hpp"
#include "database/row.hpp"

#include "object/object.hpp"

#include <stdexcept>
#include <sqlite3.h>

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

using namespace std::tr1::placeholders;

namespace oos {
  
namespace sqlite {
  
sqlite_database::sqlite_database()
  : database_impl(new sqlite_sequencer(this))
  , db_(0)
{
}

sqlite_database::~sqlite_database()
{
  close();
}


void sqlite_database::open(const std::string &db)
{
  int ret = sqlite3_open(db.c_str(), &db_);
  if (ret != SQLITE_OK) {
    throw std::runtime_error("couldn't open database: " + db);
  }

  database_impl::open(db);
}

void sqlite_database::close()
{
  database_impl::close();

  sqlite3_close(db_);
}

result_impl* sqlite_database::execute(const char *sql)
{
  sqlite_result *result = new sqlite_static_result;
  char *errmsg;
  int ret = sqlite3_exec(db_, sql, parse_result , result, &errmsg);
  return result;
}

void sqlite_database::visit(insert_action *a)
{
  // create insert statement
}

void sqlite_database::visit(update_action *a)
{
  statement_impl_ptr stmt = find_statement(std::string(a->obj()->object_type()) + "_UPDATE");
  if (!stmt) {
    // throw error
  }
  statement_binder binder;
}

void sqlite_database::visit(delete_action *a)
{
  statement_impl_ptr stmt = find_statement(std::string(a->object_type()) + "_DELETE");
  if (!stmt) {
    // throw error
  }
  statement_binder binder;
}

statement_impl* sqlite_database::create_statement()
{
  return new sqlite_statement(*this);
}

sqlite3* sqlite_database::operator()()
{
  return db_;
}

int sqlite_database::parse_result(void* param, int column_count, char** values, char** columns)
{
  sqlite_static_result *result = static_cast<sqlite_static_result*>(param);

  /********************
   *
   * a new row was retrived
   * add a new row to the result
   * and iterator over all columns
   * adding each column value as
   * string to the row
   *
   ********************/
  std::auto_ptr<row> r(new row);
  for (int i = 0; i < column_count; ++i) {
    r->push_back(values[i]);
  }
  result->push_back(r.release());
  return 0;
}

}
}

extern "C"
{

  OOS_SQLITE_API oos::database_impl* create_database()
  {
    return new oos::sqlite::sqlite_database();
  }

  OOS_SQLITE_API void destroy_database(oos::database_impl *db)
  {
    delete db;
  }
}
