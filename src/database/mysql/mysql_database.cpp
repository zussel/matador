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

#include "database/mysql/mysql_database.hpp"
#include "database/mysql/mysql_statement.hpp"
#include "database/mysql/mysql_result.hpp"
#include "database/mysql/mysql_types.hpp"
#include "database/mysql/mysql_exception.hpp"

#include "database/session.hpp"
#include "database/transaction.hpp"
#include "database/statement_creator.hpp"
#include "database/statement_reader.hpp"
#include "database/statement_binder.hpp"
#include "database/row.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

#include <stdexcept>
#include <iostream>

#include <mysql/mysql.h>

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

using namespace std::tr1::placeholders;

namespace oos {
  
namespace mysql {
  
mysql_database::mysql_database(session *db)
  : database(db, new mysql_sequencer(this))
  , mysql_db_(0)
{
}

mysql_database::~mysql_database()
{
  close();
}


void mysql_database::open(const std::string &db)
{
  if (is_open()) {
    return;
  } else {
    mysql_db_ = mysql_init(NULL);
  
    st_mysql *ret = mysql_real_connect(mysql_db_, "", "", "", NULL, 0, NULL, 0);
    database::open(db);
  }
}

bool mysql_database::is_open() const
{
  return mysql_db_ != 0;
}

void mysql_database::close()
{
  if (!is_open()) {
    return;
  } else {
    database::close();

    mysql_close(mysql_db_);

    mysql_db_ = 0;
  }
}

void mysql_database::create(const prototype_node &node)
{
  create_statement_creator<mysql_types> creator;

#ifdef WIN32
  std::auto_ptr<object> o(node.producer->create());
#else
  std::unique_ptr<object> o(node.producer->create());
#endif

    std::string sql = creator.create(o.get(), node.type.c_str(), "");

    execute(sql.c_str());
}

void mysql_database::drop(const prototype_node &node)
{
  drop_statement_creator<mysql_types> creator;

#ifdef WIN32
  std::auto_ptr<object> o(node.producer->create());
#else
  std::unique_ptr<object> o(node.producer->create());
#endif

    std::string sql = creator.create(o.get(), node.type.c_str(), "");

    execute(sql.c_str());
}

void mysql_database::load(const prototype_node &node)
{
  // create dummy object
#ifdef WIN32
  std::auto_ptr<object> o(node.producer->create());
#else
  std::unique_ptr<object> o(node.producer->create());
#endif
  // try to find select statement statement
  statement_ptr stmt = find_statement(node.type + "_SELECT");
  if (!stmt) {
    select_statement_creator<mysql_types> creator;
  // state wasn't found, create sql string
    std::string sql = creator.create(o.get(), node.type.c_str(), 0);
  // create the real statement
    stmt.reset(create_statement());
    // prepare statement
    stmt->prepare(sql);
    // store statement
    store_statement(node.type + "_SELECT", stmt);
  }

  table_info_map_t::iterator i = table_info_map().find(node.type);
  if (i == table_info_map().end()) {
    throw mysql_exception("load: couldn't find node");
  }
//  table_info_t &info = table_info_map().at(node.type);

  statement_reader reader(node, stmt, i->second);
  /* iterate over statement results and create 
   * and insert objects
   */
  reader.import();
  
  // call super class
  database::load(node);
}

void mysql_database::execute(const char *sql, result_impl *res)
{
  char *errmsg;
  mysql_query(mysql_db_, sql);
  int ret = mysql_query(mysql_db_, sql);
  if (ret != 0) {
    throw mysql_exception("error");
  }
}

void mysql_database::visit(insert_action *a)
{
  // create insert statement
  statement_ptr stmt = find_statement(std::string(a->type()) + "_INSERT");
  if (!stmt) {
    // create statement
    insert_statement_creator<mysql_types> creator;

    if (a->empty()) {
      return;
    }
    
    insert_action::const_iterator i = a->begin();
    
    std::string sql = creator.create(*i, a->type().c_str(), "");

    stmt.reset(create_statement());
    stmt->prepare(sql);
    
    store_statement(std::string(a->type()) + "_INSERT", stmt);    
  }
  statement_binder binder;

  insert_action::const_iterator first = a->begin();
  insert_action::const_iterator last = a->end();
  while (first != last) {
    object *o = (*first++);
    binder.bind(stmt, o, false);
    stmt->step();
    stmt->reset(true);
  }
}

void mysql_database::visit(update_action *a)
{
  statement_ptr stmt = find_statement(std::string(a->obj()->classname()) + "_UPDATE");
  if (!stmt) {
    // create statement
    update_statement_creator<mysql_types> creator;

    std::string sql = creator.create(a->obj(), a->obj()->classname(), "id=?");

    stmt.reset(create_statement());
    stmt->prepare(sql);
    
    store_statement(std::string(a->obj()->classname()) + "_UPDATE", stmt);    
  }

  statement_binder binder;
  binder.bind(stmt, a->obj(), true);
  stmt->step();
  stmt->reset(true);
}

void mysql_database::visit(delete_action *a)
{
  statement_ptr stmt = find_statement(std::string(a->classname()) + "_DELETE");
  if (!stmt) {
    // create statement
    delete_statement_creator<mysql_types> creator;

    std::string sql = creator.create(0, a->classname(), "id=?");

    stmt.reset(create_statement());
    stmt->prepare(sql);
    
    store_statement(std::string(a->classname()) + "_UPDATE", stmt);    
  }

  stmt->bind(1, a->id());
  stmt->step();
  stmt->reset(true);
}

result_impl* mysql_database::create_result()
{
  return new mysql_static_result;
}

statement* mysql_database::create_statement()
{
  return new mysql_statement(*this);
}

st_mysql* mysql_database::operator()()
{
  return mysql_db_;
}

void mysql_database::on_begin()
{
  execute("BEGIN TRANSACTION;");
}

void mysql_database::on_commit()
{
  execute("COMMIT TRANSACTION;");
}

void mysql_database::on_rollback()
{
  execute("ROLLBACK TRANSACTION;");
}

int mysql_database::parse_result(void* param, int column_count, char** values, char** /*columns*/)
{
  mysql_static_result *result = static_cast<mysql_static_result*>(param);

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

extern "C"
{
  OOS_MYSQL_API oos::database* create_database(oos::session *ses)
  {
    return new oos::mysql::mysql_database(ses);
  }

  OOS_MYSQL_API void destroy_database(oos::database *db)
  {
    delete db;
  }
}
