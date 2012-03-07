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
#include "database/sqlite/sqlite_types.hpp"

#include "database/database.hpp"
#include "database/transaction.hpp"
#include "database/statement_creator.hpp"
#include "database/statement_binder.hpp"
#include "database/statement_serializer.hpp"
#include "database/row.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

#include <stdexcept>
#include <sqlite3.h>
#include <iostream>

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

using namespace std::tr1::placeholders;

namespace oos {
  
namespace sqlite {
  
sqlite_database::sqlite_database(database *db, const std::string &conn)
  : database_impl(db, new sqlite_sequencer(this))
  , sqlite_db_(0)
{
}

sqlite_database::~sqlite_database()
{
  close();
}


void sqlite_database::open(const std::string &db)
{
  int ret = sqlite3_open(db.c_str(), &sqlite_db_);
  if (ret != SQLITE_OK) {
    throw std::runtime_error("couldn't open database: " + db);
  }

  database_impl::open(db);
}

bool sqlite_database::is_open() const
{
  return sqlite_db_ != 0;
}

void sqlite_database::close()
{
  database_impl::close();

  sqlite3_close(sqlite_db_);

  sqlite_db_ = 0;
}

void sqlite_database::create(const prototype_node &node)
{
  create_statement_creator<sqlite_types> creator;

  std::auto_ptr<object> o(node.producer->create());
  statement_impl *stmt = creator.create(this, o.get(), node.type, "");
}

void sqlite_database::load(const prototype_node &node)
{
  select_statement_creator<sqlite_types> creator;

  object *o = node.producer->create();

  statement_impl *stmt = creator.create(this, o, node.type, "");

//  statement_serializer ss;
  while (stmt->step()) {
  }
}

bool sqlite_database::execute(const char *sql, result_impl *res)
{
  char *errmsg;
  int ret = sqlite3_exec(sqlite_db_, sql, parse_result, res, &errmsg);
  if (ret != SQLITE_OK) {
    //throw database_error(errmsg);
    sqlite3_free(errmsg);
    return false;
  }
  return true;
}

void sqlite_database::visit(insert_action *a)
{
  // create insert statement
  statement_impl_ptr stmt = find_statement(std::string(a->type()) + "_INSERT");
  if (!stmt) {
    // throw error
  }
  statement_binder binder;

  insert_action::const_iterator first = a->begin();
  insert_action::const_iterator last = a->end();
  while (first != last) {
    object *o = (*first++);
    binder.bind(stmt.get(), o, false);
    stmt->step();
    stmt->reset(true);
  }
}

void sqlite_database::visit(update_action *a)
{
  statement_impl_ptr stmt = find_statement(std::string(a->obj()->object_type()) + "_UPDATE");
  if (!stmt) {
    // throw error
  }
  statement_binder binder;
  binder.bind(stmt.get(), a->obj(), true);
  stmt->step();
  stmt->reset(true);
}

void sqlite_database::visit(delete_action *a)
{
  statement_impl_ptr stmt = find_statement(std::string(a->object_type()) + "_DELETE");
  if (!stmt) {
    // throw error
  }
  stmt->bind(1, a->id());
  stmt->step();
  stmt->reset(true);
}

result_impl* sqlite_database::create_result()
{
  return new sqlite_static_result;
}

statement_impl* sqlite_database::create_statement()
{
  return new sqlite_statement(*this);
}

sqlite3* sqlite_database::operator()()
{
  return sqlite_db_;
}

void sqlite_database::on_begin()
{
  execute("BEGIN TRANSACTION;");
}

void sqlite_database::on_commit()
{
  execute("COMMIT TRANSACTION;");
}

void sqlite_database::on_rollback()
{
  execute("ROLLBACK TRANSACTION;");
}

int sqlite_database::parse_result(void* param, int column_count, char** values, char** /*columns*/)
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
    r->push_back(std::string(values[i]));
  }
  result->push_back(r.release());
  return 0;
}

}

}
