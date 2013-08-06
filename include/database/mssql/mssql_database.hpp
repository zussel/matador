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

#ifndef MSSQL_DATABASE_HPP
#define MSSQL_DATABASE_HPP

#ifdef WIN32
  #ifdef oos_mysql_EXPORTS
    #define OOS_MSSQL_API __declspec(dllexport)
  #else
    #define OOS_MSSQL_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_MSSQL_API
#endif

#include "database/database.hpp"

#include <sqltypes.h>

#ifdef WIN32

#else

#endif

namespace oos {
  
struct prototype_node;

namespace mssql {

class mssql_statement;

/**
 * @class mssql_database
 * @brief The mysql database backend
 * 
 * This class is the sqlite database backend
 * class. It provides the mysql version 4 and higher
 */
class OOS_MSSQL_API mssql_database : public database
{
public:
  explicit mssql_database(session *db);
  virtual ~mssql_database();
  
  /**
   * Returns true if the database is open
   *
   * @return True on open database connection.
   */
  virtual bool is_open() const;

  /**
   * Execute a sql statement and return a result
   * implementation via pointer.
   *
   * @param sql The sql statement to be executed.
   * @return The result of the executed sql statement.
   */
  virtual result* execute(const std::string &sql);

  /**
   * Create a new sqlite result
   * 
   * @return A new sqlite result
   */
  virtual result* create_result();

  /**
   * Create a new sqlite statement
   * 
   * @return A new sqlite statement
   */
  virtual statement* create_statement();
  
  virtual const char* type_string(data_type_t type) const;

protected:
  virtual void on_open(const std::string &db);
  virtual void on_close();
  virtual void on_begin();
  virtual void on_commit();
  virtual void on_rollback();

private:
  SQLHANDLE odbc_;
  SQLHANDLE connection_;

  bool is_open_;
};

}

}

extern "C"
{
  OOS_MSSQL_API oos::database* create_database(oos::session *ses);

  OOS_MSSQL_API void destroy_database(oos::database *db);
}

#endif /* MSSQL_DATABASE_HPP */
