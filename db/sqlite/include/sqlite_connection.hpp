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

#ifndef SQLITE_DATABASE_HPP
#define SQLITE_DATABASE_HPP

#ifdef WIN32
  #ifdef oos_sqlite_EXPORTS
    #define OOS_SQLITE_API __declspec(dllexport)
  #else
    #define OOS_SQLITE_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_SQLITE_API
#endif

#include "sqlite_dialect.hpp"

#include "sql/statement_impl.hpp"
#include "sql/connection_impl.hpp"
#include "sql/result_impl.hpp"

struct sqlite3;

namespace oos {
  
class prototype_node;

namespace sqlite {

class sqlite_statement;

/**
 * @class sqlite_database
 * @brief The sqlite sql backend
 * 
 * This class is the sqlite sql backend
 * class. It provides the sqlite version 3
 */
class OOS_SQLITE_API sqlite_connection : public connection_impl
{
public:
  sqlite_connection();
  virtual ~sqlite_connection();
  
  /**
   * Returns true if the sql is open
   *
   * @return True on open sql connection.
   */
  virtual bool is_open() const override;

  virtual unsigned long last_inserted_id();

  virtual void open(const std::string &db);
  virtual void close() override;
  detail::result_impl* execute(const oos::sql &stmt);
  detail::result_impl* execute(const std::string &stmt);
  detail::statement_impl* prepare(const oos::sql &stmt);
  virtual void begin() override;
  virtual void commit() override;
  virtual void rollback() override;

  /**
   * Return the raw pointer to the sqlite3
   * database struct.
   * 
   * @return The raw sqlite3 pointer.
   */
  sqlite3* operator()();

private:
  static int parse_result(void* param, int column_count, char** values, char** columns);

private:
  sqlite3 *sqlite_db_;
  sqlite_dialect dialect_;
};

}

}

extern "C"
{
  OOS_SQLITE_API oos::connection_impl* create_database();

  OOS_SQLITE_API void destroy_database(oos::connection_impl *db);
}

#endif /* SQLITE_DATABASE_HPP */
