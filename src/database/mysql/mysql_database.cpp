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
#include "database/database_sequencer.hpp"
#include "database/row.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

#include <stdexcept>
#include <iostream>
#include <sstream>

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

using namespace std::tr1::placeholders;

namespace oos {
  
namespace mysql {
  
mysql_database::mysql_database(session *db)
  : database(db, new database_sequencer(*this))
  , is_open_(false)
{
}

mysql_database::~mysql_database()
{
  close();
}


void mysql_database::on_open(const std::string &connection)
{
  // parse user[:passwd]@host/db
  
  std::string con = connection;
  std::string::size_type pos = con.find('@');
  std::string user, passwd;
  bool has_pwd = true;
  if (pos == std::string::npos) {
	  throw_error("mysql:open", "invalid dsn: " + connection);
  } else {
    // try to find colon (:)
    std::string credentials = con.substr(0, pos);
    std::string::size_type colpos = credentials.find(':');
    if (colpos != std::string::npos) {
      // found colon, extract user and passwd
      user = credentials.substr(0, colpos);
      passwd = credentials.substr(colpos + 1, pos);
    } else {
      // only user name given
      user = credentials.substr(0, pos);
      has_pwd = false;
    }
  }
  con = con.substr(pos + 1);
  pos = con.find('/');
  std::string host = con.substr(0, pos);
  std::string db = con.substr(pos + 1);
  mysql_init(&mysql_);
//  if (mysql_real_connect(&mysql_, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), 0, NULL, 0) == NULL) {
  if (mysql_real_connect(&mysql_, host.c_str(), user.c_str(), (has_pwd ? passwd.c_str() : 0), db.c_str(), 0, NULL, 0) == NULL) {
    printf("Connection Error %u: %s\n", mysql_errno(&mysql_), mysql_error(&mysql_));
    exit(1);
  }
  is_open_ = true;
}

bool mysql_database::is_open() const
{
  return is_open_;
}

void mysql_database::on_close()
{
//  std::cout << "closing database\n";
  mysql_close(&mysql_);
  // tell mysql to close the library
  mysql_library_end();

  is_open_ = false;
}

result* mysql_database::execute(const std::string &sqlstr)
{
//  std::cout << "Executing statement: " << sqlstr << "\n";
  if (mysql_query(&mysql_, sqlstr.c_str())) {
    throw mysql_exception(&mysql_, "mysql_query", sqlstr);
  }
  return new mysql_result(&mysql_);
}

result* mysql_database::create_result()
{
  return new mysql_result(&mysql_);
}

statement* mysql_database::create_statement()
{
  return new mysql_statement(*this);
}

MYSQL* mysql_database::operator()()
{
  return &mysql_;
}

void mysql_database::on_begin()
{
  result *res = execute("START TRANSACTION;");
  // TODO: check result
  delete res;
}

void mysql_database::on_commit()
{
  result *res = execute("COMMIT;");
  // TODO: check result
  delete res;
}

void mysql_database::on_rollback()
{
  result *res = execute("ROLLBACK;");
  // TODO: check result
  delete res;
}

const char* mysql_database::type_string(data_type_t type) const
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
      return "FLOAT";
    case type_double:
      return "DOUBLE";
    case type_char_pointer:
      return "VARCHAR";
    case type_varchar:
      return "VARCHAR";
    case type_text:
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
