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

#ifndef MYSQL_DATABASE_HPP
#define MYSQL_DATABASE_HPP

#ifdef WIN32
  #ifdef oos_mysql_EXPORTS
    #define OOS_MYSQL_API __declspec(dllexport)
  #else
    #define OOS_MYSQL_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_MYSQL_API
#endif

#include "oos/sql/connection_impl.hpp"
#include "oos/sql/types.hpp"

#include "oos/db/mysql/mysql_dialect.hpp"

#ifdef WIN32
//#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

namespace oos {
  
namespace mysql {

class mysql_statement;

/**
 * @class mysql_database
 * @brief The mysql sql backend
 * 
 * This class is the sqlite sql backend
 * class. It provides the mysql version 4 and higher
 */
class OOS_MYSQL_API mysql_connection : public connection_impl
{
public:
  mysql_connection();
  virtual ~mysql_connection();
  
  /**
   * Returns true if the sql is open
   *
   * @return True on open sql connection.
   */
  virtual bool is_open() const override;

  virtual unsigned long last_inserted_id();

  /**
   * Return the raw pointer to the sqlite3
   * database struct.
   * 
   * @return The raw sqlite3 pointer.
   */
  MYSQL* handle();

  virtual void open(const std::string &db) override;
  virtual void close() override;

  virtual detail::result_impl* execute(const oos::sql &stmt) override;
  virtual detail::result_impl* execute(const std::string &stmt) override;
  virtual detail::statement_impl* prepare(const oos::sql &stmt) override;

  virtual void begin() override;
  virtual void commit() override;
  virtual void rollback() override;

  virtual std::string type() const override;
  virtual std::string version() const override;

  virtual bool exists(const std::string &tablename) override;
  virtual std::vector<field> describe(const std::string &table) override;

  virtual basic_dialect* dialect() override;

private:
  MYSQL mysql_;
  std::string db_;
  bool is_open_;
  mysql_dialect dialect_;
};

}

}

extern "C"
{
  OOS_MYSQL_API oos::connection_impl* create_database();

  OOS_MYSQL_API void destroy_database(oos::connection_impl *db);
}

#endif /* MYSQL_DATABASE_HPP */
