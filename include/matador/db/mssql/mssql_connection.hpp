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

#ifndef MSSQL_connection_HPP
#define MSSQL_connection_HPP

#ifdef _MSC_VER
  #ifdef oos_mssql_EXPORTS
    #define OOS_MSSQL_API __declspec(dllexport)
  #else
    #define OOS_MSSQL_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_MSSQL_API
#endif

#if defined(_MSC_VER)
#include <windows.h>
#endif

#include "matador/db/mssql/mssql_dialect.hpp"

#include "matador/sql/connection_impl.hpp"

#include <sqltypes.h>

namespace matador {
namespace mssql {

class mssql_statement;

/**
 * @class mssql_connection
 * @brief The mysql connection backend
 * 
 * This class is the sqlite connection backend
 * class. It provides the mysql version 4 and higher
 */
class OOS_MSSQL_API mssql_connection : public connection_impl
{
public:
  explicit mssql_connection();
  virtual ~mssql_connection();

  virtual void open(const std::string &db) override;
  virtual void close() override;

  /**
   * Returns true if the connection is open
   *
   * @return True on open connection connection.
   */
  virtual bool is_open() const override;

  virtual matador::detail::result_impl* execute(const matador::sql &stmt) override;
  virtual matador::detail::result_impl* execute(const std::string &stmt) override;
  virtual matador::detail::statement_impl* prepare(const matador::sql &stmt) override;

  virtual void begin() override;
  virtual void commit() override;
  virtual void rollback() override;

  virtual std::string type() const override;
  virtual std::string version() const override;

  virtual bool exists(const std::string &tablename) override;
  virtual std::vector<field> describe(const std::string &table) override;

  SQLHANDLE handle();

  virtual unsigned long last_inserted_id();

  virtual basic_dialect* dialect() override;

private:
  void execute_no_result(const std::string &stmt);

private:
  SQLHANDLE odbc_;
  SQLHANDLE connection_;

  std::string db_;

  mssql_dialect dialect_;
  bool is_open_;
};

}

}

extern "C"
{
  OOS_MSSQL_API matador::connection_impl* create_connection();

  OOS_MSSQL_API void destroy_connection(matador::connection_impl *db);
}

#endif /* MSSQL_connection_HPP */
