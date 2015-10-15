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

#include "sqlite_database.hpp"
#include "sqlite_statement.hpp"
#include "sqlite_result.hpp"
#include "sqlite_types.hpp"
#include "sqlite_exception.hpp"

#include "database/session.hpp"
#include "database/result.hpp"

#include "database/database_sequencer.hpp"

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

sqlite_database::sqlite_database(session *db)
  : database(db, new database_sequencer(*this))
  , sqlite_db_(0)
{
}

sqlite_database::~sqlite_database()
{
  close();
}


void sqlite_database::on_open(const std::string &db)
{
  int ret = sqlite3_open(db.c_str(), &sqlite_db_);
  if (ret != SQLITE_OK) {
    throw sqlite_exception("couldn't open database: " + db);
  }
}

bool sqlite_database::is_open() const
{
  return sqlite_db_ != 0;
}

void sqlite_database::on_close()
{
  int ret = sqlite3_close(sqlite_db_);
  
  throw_error(ret, sqlite_db_, "sqlite_close");

  sqlite_db_ = 0;
}

sqlite3* sqlite_database::operator()()
{
  return sqlite_db_;
}

void sqlite_database::on_begin()
{
  execute<serializable>("BEGIN TRANSACTION;", (std::shared_ptr<object_base_producer>()));
}

void sqlite_database::on_commit()
{
  execute<serializable>("COMMIT TRANSACTION;", (std::shared_ptr<object_base_producer>()));
}

oos::detail::result_impl* sqlite_database::on_execute(const std::string &sql, std::shared_ptr<object_base_producer> ptr)
{
  std::unique_ptr<sqlite_result> res(new sqlite_result(ptr));
  char *errmsg = 0;
  int ret = sqlite3_exec(sqlite_db_, sql.c_str(), parse_result, res.get(), &errmsg);
  if (ret != SQLITE_OK) {
    std::string error(errmsg);
    sqlite3_free(errmsg);
    throw sqlite_exception(error);
  }
  return res.release();
}

oos::detail::statement_impl *sqlite_database::on_prepare(const oos::sql &sql, std::shared_ptr<object_base_producer> ptr)
{
  return new sqlite_statement(*this, sql.prepare(), ptr);
}

void sqlite_database::on_rollback()
{
  execute<serializable>("ROLLBACK TRANSACTION;", (std::shared_ptr<object_base_producer>()));
}

int sqlite_database::parse_result(void* param, int column_count, char** values, char** /*columns*/)
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

const char* sqlite_database::type_string(data_type_t type) const
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
        msg << "mysql database: unknown type xxx [" << type << "]";
        throw std::logic_error(msg.str());
        //throw std::logic_error("mysql database: unknown type");
      }
  }
}

unsigned long sqlite_database::last_inserted_id()
{
    return static_cast<unsigned long>(sqlite3_last_insert_rowid(sqlite_db_));
}

}

}

extern "C"
{
  OOS_SQLITE_API oos::database* create_database(oos::session *ses)
  {
    return new oos::sqlite::sqlite_database(ses);
  }

  OOS_SQLITE_API void destroy_database(oos::database *db)
  {
    delete db;
  }
}
