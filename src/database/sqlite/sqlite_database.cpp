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
#include "database/sqlite/sqlite_exception.hpp"

#include "database/session.hpp"
#include "database/transaction.hpp"
#include "database/row.hpp"

#include "database/statement_creator.hpp"
#include "database/database_sequencer.hpp"

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
  sqlite3_close(sqlite_db_);

  sqlite_db_ = 0;
}

result* sqlite_database::execute(const std::string &sql)
{
  std::cout << "executing sql [" << sql << "]\n";
  std::unique_ptr<sqlite_result> res(new sqlite_result);
  char *errmsg;
  int ret = sqlite3_exec(sqlite_db_, sql.c_str(), parse_result, res.get(), &errmsg);
  if (ret != SQLITE_OK) {
    std::string error(errmsg);
    sqlite3_free(errmsg);
    throw sqlite_exception(error);
  }
  return res.release();
}

statement* sqlite_database::create_statement()
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
  sqlite_result *result = static_cast<sqlite_result*>(param);

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

const char* sqlite_database::type_string(sql::data_type_t type) const
{
  switch(type) {
    case sql::type_char:
      return "INTEGER";
    case sql::type_short:
      return "INTEGER";
    case sql::type_int:
      return "INTEGER";
    case sql::type_long:
      return "INTEGER";
    case sql::type_unsigned_char:
      return "INTEGER";
    case sql::type_unsigned_short:
      return "INTEGER";
    case sql::type_unsigned_int:
      return "INTEGER";
    case sql::type_unsigned_long:
      return "INTEGER";
    case sql::type_bool:
      return "INTEGER";
    case sql::type_float:
      return "DOUBLE";
    case sql::type_double:
      return "DOUBLE";
    case sql::type_text:
      return "VARCHAR(64)";
    default:
      throw std::logic_error("unknown type");
  }
}

result *sqlite_database::create_result()
{
  return 0;
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
