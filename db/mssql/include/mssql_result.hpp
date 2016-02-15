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
class serializable;

namespace mssql {

class mssql_result : public detail::result_impl
{
private:
  mssql_result(const mssql_result&) = delete;
  mssql_result& operator=(const mssql_result&) = delete;

public:
  mssql_result(SQLHANDLE stmt, bool free);
  virtual ~mssql_result();
  
  const char* column(size_type c) const;
  virtual bool fetch();
  size_type affected_rows() const;
  size_type result_rows() const;
  size_type fields() const;

  virtual int transform_index(int index) const;

protected:
  virtual void serialize(const char*, char&);
  virtual void serialize(const char*, short&);
  virtual void serialize(const char*, int&);
  virtual void serialize(const char*, long&);
  virtual void serialize(const char*, unsigned char&);
  virtual void serialize(const char*, unsigned short&);
  virtual void serialize(const char*, unsigned int&);
  virtual void serialize(const char*, unsigned long&);
  virtual void serialize(const char*, bool&);
  virtual void serialize(const char*, float&);
  virtual void serialize(const char*, double&);
  virtual void serialize(const char*, char *, size_t);
  virtual void serialize(const char*, std::string&);
  virtual void serialize(const char*, oos::varchar_base&);
  virtual void serialize(const char*, oos::time&);
  virtual void serialize(const char*, oos::date&);
  virtual void serialize(const char*, oos::basic_identifier &x);
  virtual void serialize(const char*, oos::identifiable_holder &x, cascade_type);

  template < class T >
  void read_column(const char *, T & val)
  {
    SQLLEN info = 0;
    SQLSMALLINT type = (SQLSMALLINT)mssql_statement::type2int(type_traits<T>::data_type());
    SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)(result_index_++), type, &val, sizeof(T), &info);
    if (ret == SQL_SUCCESS) {
      return;
    } else {
      throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "error on retrieving field value");
    }
  }
  
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
  
  bool free_;
  
  enum { NUMERIC_LEN = 21 };

  SQLHANDLE stmt_;
};

}

}

#endif /* MSSQL_RESULT_HPP */
