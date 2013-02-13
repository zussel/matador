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
#include "database/mysql/mysql_table.hpp"
#include "database/mysql/mysql_types.hpp"
#include "database/mysql/mysql_exception.hpp"
#include "database/mysql/mysql_sequencer.hpp"

#include "database/session.hpp"
#include "database/transaction.hpp"
#include "database/statement_creator.hpp"
#include "database/row.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

#include <stdexcept>
#include <iostream>

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
  , is_open_(false)
{
}

mysql_database::~mysql_database()
{
  close();
  // tell mysql to close the library
  mysql_library_end();
}


void mysql_database::open(const std::string &db)
{
  if (is_open()) {
    return;
  } else {
    if (::mysql_init(&mysql_) == 0) {
      std::stringstream msg;
      msg << "mysql" << ": " << mysql_error(&mysql_) << "(" << db << ")";
      throw mysql_exception(msg.str());
    }  
    if (0 == mysql_real_connect(&mysql_, "localhost", "sascha", "sascha", "test", 0, NULL, 0)) {
      std::stringstream msg;
      msg << "mysql" << ": " << mysql_error(&mysql_) << "(" << db << ")";
      throw mysql_exception(msg.str());
    }
    database::open(db);

    is_open_ = true;
  }
}

bool mysql_database::is_open() const
{
  return is_open_;
}

void mysql_database::close()
{
  if (!is_open()) {
    return;
  } else {
    database::close();

    std::cout << "closing database\n";
    mysql_close(&mysql_);

    is_open_ = false;
  }
}

void mysql_database::execute(const char *sql, result_impl */*res*/)
{
  std::cout << "executing: " << sql << "\n";
  if (mysql_query(&mysql_, sql)) {
    std::stringstream msg;
    msg << "mysql" << ": " << mysql_error(&mysql_) << "(" << sql << ")";
    throw mysql_exception(msg.str());
  }
}

result_impl* mysql_database::create_result()
{
  return new mysql_static_result;
}

statement* mysql_database::create_statement()
{
  return new mysql_statement(*this);
}

table* mysql_database::create_table(const prototype_node &node)
{
  return new mysql_table(*this, node);
}

MYSQL* mysql_database::operator()()
{
  return &mysql_;
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
    printf("deleting database\n");
    delete db;
  }
}
