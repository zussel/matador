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

#include "sqlite_connection.hpp"
#include "sqlite_statement.hpp"
#include "sqlite_result.hpp"
#include "sqlite_types.hpp"
#include "sqlite_exception.hpp"

#include "sql/sql.hpp"

#include <sqlite3.h>
#include <sstream>

using namespace std::placeholders;

namespace oos {
  
namespace sqlite {
  
void throw_error(int ec, sqlite3 *db, const std::string &source)
{
  if (ec == SQLITE_OK) {
    return;
  }
  std::stringstream msg;
  msg << source << ": " << sqlite3_errmsg(db);
  throw sqlite_exception(msg.str()); 
}

sqlite_connection::sqlite_connection()
  : sqlite_db_(0)
{
}

sqlite_connection::~sqlite_connection()
{
  close();
}


void sqlite_connection::open(const std::string &db)
{
  int ret = sqlite3_open(db.c_str(), &sqlite_db_);
  if (ret != SQLITE_OK) {
    throw sqlite_exception("couldn't open database: " + db);
  }
}

bool sqlite_connection::is_open() const
{
  return sqlite_db_ != 0;
}

void sqlite_connection::close()
{
  int ret = sqlite3_close(sqlite_db_);
  
  throw_error(ret, sqlite_db_, "sqlite_close");

  sqlite_db_ = 0;
}

sqlite3*sqlite_connection::operator()()
{
  return sqlite_db_;
}

void sqlite_connection::begin()
{
  execute("BEGIN TRANSACTION;");
}

void sqlite_connection::commit()
{
  execute("COMMIT TRANSACTION;");
}

void sqlite_connection::rollback()
{
  execute("ROLLBACK TRANSACTION;");
}

oos::detail::result_impl* sqlite_connection::execute(const std::string &sql)
{
  std::unique_ptr<sqlite_result> res(new sqlite_result);
  char *errmsg = 0;
  int ret = sqlite3_exec(sqlite_db_, sql.c_str(), parse_result, res.get(), &errmsg);
  if (ret != SQLITE_OK) {
    std::string error(errmsg);
    sqlite3_free(errmsg);
    throw sqlite_exception(error);
  }
  return res.release();
}

oos::detail::statement_impl *sqlite_connection::prepare(const oos::sql &sql)
{
  return new sqlite_statement(*this, sql.prepare());
}

int sqlite_connection::parse_result(void* param, int column_count, char** values, char** /*columns*/)
{
  sqlite_result *result = static_cast<sqlite_result*>(param);

  /********************
   *
   * a new row was retrieved
   * add a new row to the result
   * and iterator over all columns
   * adding each column value as
   * string to the row
   *
   ********************/

  result->push_back(values, column_count);
  return 0;
}

const char*sqlite_connection::type_string(data_type_t type) const
{
  switch(type) {
    case type_char:
      return "INTEGER";
    case type_short:
      return "INTEGER";
    case type_int:
      return "INTEGER";
    case type_long:
      return "INTEGER";
    case type_unsigned_char:
      return "INTEGER";
    case type_unsigned_short:
      return "INTEGER";
    case type_unsigned_int:
      return "INTEGER";
    case type_unsigned_long:
      return "INTEGER";
    case type_bool:
      return "INTEGER";
    case type_float:
      return "DOUBLE";
    case type_double:
      return "DOUBLE";
    case type_char_pointer:
      return "VARCHAR";
    case type_varchar:
      return "VARCHAR";
    case type_text:
      return "TEXT";
    case type_date:
      return "REAL";
    case type_time:
      return "TEXT";
    default:
      {
        std::stringstream msg;
        msg << "sqlite database: unknown type xxx [" << type << "]";
        throw std::logic_error(msg.str());
        //throw std::logic_error("mysql database: unknown type");
      }
  }
}

unsigned long sqlite_connection::last_inserted_id()
{
  return static_cast<unsigned long>(sqlite3_last_insert_rowid(sqlite_db_));
}

}

}

extern "C"
{
  OOS_SQLITE_API oos::connection_impl* create_database()
  {
    return new oos::sqlite::sqlite_connection();
  }

  OOS_SQLITE_API void destroy_database(oos::connection_impl *db)
  {
    delete db;
  }
}
