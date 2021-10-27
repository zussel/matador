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

#ifndef MSSQL_RESULT_HPP
#define MSSQL_RESULT_HPP

#include "matador/db/mssql/mssql_exception.hpp"
#include "matador/db/mssql/mssql_statement.hpp"
#include "matador/db/mssql/mssql_types.hpp"

#include "matador/sql/result_impl.hpp"
#include "matador/sql/types.hpp"

#include <vector>

#include <sqltypes.h>
#include <sql.h>

namespace matador {

class row;

namespace mssql {

class mssql_result : public detail::result_impl
{
public:
  mssql_result(const mssql_result&) = delete;
  mssql_result& operator=(const mssql_result&) = delete;

public:
  explicit mssql_result(SQLHANDLE stmt);
  ~mssql_result() override;

  const char* column(size_type c) const override;
  bool fetch() override;

  size_type affected_rows() const override;
  size_type result_rows() const override;
  size_type fields() const override;

  int transform_index(int index) const override;

  template < class T >
  T get(size_type index, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr) const
  {
    T val;
    SQLLEN info = 0;
    auto type = (SQLSMALLINT)type2int(matador::data_type_traits<T>::builtin_type());
    SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)(index), type, &val, sizeof(T), &info);
    if (!SQL_SUCCEEDED(ret)) {
      throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
    }
    return val;
  }

  template < class T >
  T get(size_type index, typename std::enable_if<std::is_same<std::string, T>::value>::type* = nullptr) const
  {
    char buf[1024];
    SQLLEN info = 0;
    SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)(index), SQL_C_CHAR, buf, 1024, &info);
    if (!SQL_SUCCEEDED(ret)) {
      throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
    }
    return std::string(buf, info);
  }

protected:
  void serialize(const char*, char&) override;
  void serialize(const char*, short&) override;
  void serialize(const char*, int&) override;
  void serialize(const char*, long&) override;
  void serialize(const char*, long long&) override;
  void serialize(const char*, unsigned char&) override;
  void serialize(const char*, unsigned short&) override;
  void serialize(const char*, unsigned int&) override;
  void serialize(const char*, unsigned long&) override;
  void serialize(const char*, unsigned long long&) override;
  void serialize(const char*, bool&) override;
  void serialize(const char*, float&) override;
  void serialize(const char*, double&) override;
  void serialize(const char*, char *, size_t) override;
  void serialize(const char*, std::string&) override;
  void serialize(const char*, std::string &x, size_t s) override;
  void serialize(const char*, matador::time&) override;
  void serialize(const char*, matador::date&) override;
  void serialize(const char*, matador::basic_identifier &x) override;
  void serialize(const char*, matador::identifiable_holder &x, cascade_type) override;

  template < class T >
  void read_column(const char *id, T & val)
  {
    SQLLEN info = 0;
    auto type = (SQLSMALLINT)type2int(data_type_traits<T>::builtin_type());
    SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)(result_index_++), type, &val, sizeof(T), &info);
    if (SQL_SUCCEEDED(ret)) {
      return;
    } else {
      std::stringstream msg;
      msg << "error on retrieving value for column " << id << " (type " << typeid(T).name() << ")";
      throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", msg.str());
    }
  }
  
  void read_column(const char *, char &val);
  void read_column(const char *, unsigned char &val);
  void read_column(const char *, std::string &val);
  void read_column(const char *, std::string &val, size_t);
  void read_column(const char *, matador::date &val);
  void read_column(const char *, matador::time &val);


  bool prepare_fetch() override;

  bool finalize_fetch() override;

private:
  size_type affected_rows_ = 0;
  size_type rows = 0;
  size_type fields_ = 0;
  
  enum { NUMERIC_LEN = 21 };

  SQLHANDLE stmt_;
};

}

}

#endif /* MSSQL_RESULT_HPP */
