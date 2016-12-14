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

#include "oos/db/mysql/mysql_connection.hpp"
#include "oos/db/mysql/mysql_statement.hpp"
#include "oos/db/mysql/mysql_result.hpp"
#include "oos/db/mysql/mysql_types.hpp"
#include "oos/db/mysql/mysql_exception.hpp"

#include <sstream>
#include <regex>

using namespace std::placeholders;

namespace oos {
  
namespace mysql {
  
mysql_connection::mysql_connection()
  : is_open_(false)
{
}

mysql_connection::~mysql_connection()
{
  close();
}


void mysql_connection::open(const std::string &connection)
{
  // dns syntax:
  // user[:passwd]@host[:port]/db
  static const std::regex DNS_RGX("(.+?)(?::(.+?))?@([^:]+?)(?::([1-9][0-9]*?))?/(.+)");
  std::smatch what;

  if (!std::regex_match(connection, what, DNS_RGX)) {
    throw_error("mysql:open", "invalid dns: " + connection);
  }

  const std::string user = what[1].str();
  const std::string passwd = what[2].str();
  const std::string host = what[3].str();

  unsigned port = 0;
  if (what[4].matched) {
    port = unsigned(std::stoi(what[4].str()));
  }

  db_ = what[5].str();


  if (!mysql_init(&mysql_)) {
    throw_error("mysql", "initialization failed");
  }

  if (!mysql_real_connect(&mysql_, host.c_str(), user.c_str(), !passwd.empty() ? passwd.c_str() : nullptr, db_.c_str(), port, nullptr, 0)) {
    // close all handles
    mysql_close(&mysql_);
    // throw exception
    throw_error(-1, &mysql_, "mysql");
  }
  is_open_ = true;
}

bool mysql_connection::is_open() const
{
  return is_open_;
}

void mysql_connection::close()
{
  if (is_open_) {
    mysql_close(&mysql_);
  }

  // tell mysql to close the library
  mysql_library_end();

  is_open_ = false;
}

MYSQL*mysql_connection::handle()
{
  return &mysql_;
}

detail::result_impl* mysql_connection::execute(const oos::sql &sql)
{
  std::string stmt = dialect_.direct(sql);
  return execute(stmt);
}

detail::result_impl* mysql_connection::execute(const std::string &stmt)
{
  if (mysql_query(&mysql_, stmt.c_str())) {
    throw mysql_exception(&mysql_, "mysql_query", stmt);
  }
  return new mysql_result(&mysql_);
}

detail::statement_impl* mysql_connection::prepare(const oos::sql &stmt)
{
  return new mysql_statement(*this, stmt);
}

void mysql_connection::begin()
{
  // TODO: check result
  std::unique_ptr<mysql_result> res(static_cast<mysql_result*>(execute("START TRANSACTION;")));
}

void mysql_connection::commit()
{
  // TODO: check result
  std::unique_ptr<mysql_result> res(static_cast<mysql_result*>(execute("COMMIT;")));
}

void mysql_connection::rollback()
{
  // TODO: check result
  std::unique_ptr<mysql_result> res(static_cast<mysql_result*>(execute("ROLLBACK;")));
}

std::string mysql_connection::type() const
{
  return "mysql";
}

std::string mysql_connection::version() const
{
  return mysql_get_server_info(const_cast<MYSQL*>(&mysql_));
}

bool mysql_connection::exists(const std::string &tablename)
{
  std::string stmt("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '" + db_ + "' AND table_name = '" + tablename + "' LIMIT 1");
  std::unique_ptr<mysql_result> res(static_cast<mysql_result*>(execute(stmt)));
  if (!res->fetch()) {
    return false;
  } else {
    char *end;
    return strtoul(res->column(0), &end, 10) == 1;
  }
}

std::vector<field> mysql_connection::describe(const std::string &table)
{
  std::string stmt("SHOW COLUMNS FROM " + table);
  std::unique_ptr<mysql_result> res(static_cast<mysql_result*>(execute(stmt)));

  std::vector<field> fields;

  while (res->fetch()) {
    field f;
    f.name(res->column(0));
    f.type(dialect_.string_type(res->column(1)));
    f.index(fields.size());
    char *end;
    f.not_null(strtoul(res->column(2), &end, 10) == 0);

    fields.push_back(f);
  }

  return fields;
}

basic_dialect* mysql_connection::dialect()
{
  return &dialect_;
}

unsigned long mysql_connection::last_inserted_id()
{
  if (mysql_field_count(&mysql_) == 0 &&
      mysql_insert_id(&mysql_) != 0)
  {
    return (unsigned long)mysql_insert_id(&mysql_);
  }
  return 0;
}

}

}

extern "C"
{
  OOS_MYSQL_API oos::connection_impl* create_database()
  {
    return new oos::mysql::mysql_connection();
  }

  OOS_MYSQL_API void destroy_database(oos::connection_impl *db)
  {
    delete db;
  }
}
