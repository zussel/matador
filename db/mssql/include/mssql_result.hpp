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

#include "mssql_exception.hpp"
#include "mssql_statement.hpp"

#include "sql/result_impl.hpp"
#include "sql/types.hpp"

#if defined(_MSC_VER)
#include <windows.h>
#endif

#include <vector>

#include <sqltypes.h>
#include <sql.h>

namespace oos {

class row;

namespace mssql {

class mssql_result : public detail::result_impl
{
private:
  mssql_result(const mssql_result&) = delete;
  mssql_result& operator=(const mssql_result&) = delete;

public:
  explicit mssql_result(SQLHANDLE stmt);
  virtual ~mssql_result();

  virtual const char* column(size_type c) const override;
  virtual bool fetch() override;

  virtual size_type affected_rows() const override;
  virtual size_type result_rows() const override;
  virtual size_type fields() const override;

  virtual int transform_index(int index) const override;

  template < class T >
  T get(size_type index, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr) const
  {
    T val;
    SQLLEN info = 0;
    SQLSMALLINT type = (SQLSMALLINT)mssql_statement::type2int(type_traits<T>::data_type());
    SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)(index), type, &val, sizeof(T), &info);
    if (!SQL_SUCCEEDED(ret)) {
      throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "error on retrieving column value");
    }
    return val;
  }

  template < class T >
  T get(size_type index, typename std::enable_if<std::is_same<std::string, T>::value>::type* = nullptr) const
  {
    char buf[1024];
    SQLLEN info = 0;
    SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)index, SQL_C_CHAR, buf, 1024, &info);
    if (!SQL_SUCCEEDED(ret)) {
      throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "error on retrieving column value");
    }
    return std::string(buf, info);
  }

protected:
  virtual void serialize(const char*, char&) override;
  virtual void serialize(const char*, short&) override;
  virtual void serialize(const char*, int&) override;
  virtual void serialize(const char*, long&) override;
  virtual void serialize(const char*, unsigned char&) override;
  virtual void serialize(const char*, unsigned short&) override;
  virtual void serialize(const char*, unsigned int&) override;
  virtual void serialize(const char*, unsigned long&) override;
  virtual void serialize(const char*, bool&) override;
  virtual void serialize(const char*, float&) override;
  virtual void serialize(const char*, double&) override;
  virtual void serialize(const char*, char *, size_t) override;
  virtual void serialize(const char*, std::string&) override;
  virtual void serialize(const char*, oos::varchar_base&) override;
  virtual void serialize(const char*, oos::time&) override;
  virtual void serialize(const char*, oos::date&) override;
  virtual void serialize(const char*, oos::basic_identifier &x) override;
  virtual void serialize(const char*, oos::identifiable_holder &x, cascade_type) override;

  template < class T >
  void read_column(const char *, T & val)
  {
    SQLLEN info = 0;
    SQLSMALLINT type = (SQLSMALLINT)mssql_statement::type2int(type_traits<T>::data_type());
    SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)(result_index_++), type, &val, sizeof(T), &info);
    if (SQL_SUCCEEDED(ret)) {
      return;
    } else {
      throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "error on retrieving column value");
    }
  }
  
  void read_column(const char *, char &val);
  void read_column(const char *, std::string &val);
  void read_column(const char *, varchar_base &val);
  void read_column(const char *, oos::date &val);
  void read_column(const char *, oos::time &val);


  virtual bool prepare_fetch() override;

  virtual bool finalize_fetch() override;

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
