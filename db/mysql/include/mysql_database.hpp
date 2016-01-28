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

#include "sql/connection_impl.hpp"
#include "sql/types.hpp"

#ifdef WIN32
#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

namespace oos {
  
class prototype_node;

namespace mysql {

class mysql_statement;

/**
 * @class mysql_database
 * @brief The mysql database backend
 * 
 * This class is the sqlite database backend
 * class. It provides the mysql version 4 and higher
 */
class OOS_MYSQL_API mysql_database : public connection_impl
{
public:
  mysql_database();
  virtual ~mysql_database();
  
  /**
   * Returns true if the database is open
   *
   * @return True on open database connection.
   */
  virtual bool is_open() const;

  virtual unsigned long last_inserted_id();

  virtual const char* type_string(data_type_t type) const;

/**
   * Return the raw pointer to the sqlite3
   * database struct.
   * 
   * @return The raw sqlite3 pointer.
   */
  MYSQL* operator()();

  virtual void open(const std::string &db);
  virtual void close() override;
  detail::result_impl* execute(const std::string &sqlstr);
  detail::statement_impl* prepare(const oos::sql &stmt);
  virtual void on_begin() override;
  virtual void on_commit() override;
  virtual void on_rollback() override;

private:
  MYSQL mysql_;
  bool is_open_;
};

}

}

extern "C"
{
  OOS_MYSQL_API oos::database* create_database(oos::session *ses);

  OOS_MYSQL_API void destroy_database(oos::database *db);
}

#endif /* MYSQL_DATABASE_HPP */
