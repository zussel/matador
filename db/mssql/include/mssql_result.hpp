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

#include "database/result.hpp"
#include "database/types.hpp"

#if defined(_MSC_VER)
#include <windows.h>
#endif

#include <sqltypes.h>
#include <sql.h>

#include <vector>
#include "object/identifier.hpp"

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
  mssql_result(SQLHANDLE stmt, bool free, std::shared_ptr<oos::object_base_producer> producer);
  virtual ~mssql_result();
  
  const char* column(size_type c) const;
  virtual bool fetch();
  virtual bool fetch(serializable *);
  size_type affected_rows() const;
  size_type result_rows() const;
  size_type fields() const;

  virtual int transform_index(int index) const;

protected:
  virtual void read(const char *id, char &x);
  virtual void read(const char *id, short &x);
  virtual void read(const char *id, int &x);
  virtual void read(const char *id, long &x);
  virtual void read(const char *id, unsigned char &x);
  virtual void read(const char *id, unsigned short &x);
  virtual void read(const char *id, unsigned int &x);
  virtual void read(const char *id, unsigned long &x);
  virtual void read(const char *id, bool &x);
  virtual void read(const char *id, float &x);
  virtual void read(const char *id, double &x);
  virtual void read(const char *id, char *x, size_t s);
  virtual void read(const char *id, varchar_base &x);
  virtual void read(const char *id, std::string &x);
  virtual void read(const char *id, oos::date &x);
  virtual void read(const char *id, oos::time &x);
  virtual void read(const char *id, object_base_ptr &x);
  virtual void read(const char *id, object_container &x);
  virtual void read(const char *id, basic_identifier &x);

  template < class T >
  void read_column(const char *, T & val)
  {
    SQLLEN info = 0;
    SQLSMALLINT type = (SQLSMALLINT)mssql_statement::type2int(type_traits<T>::data_type());
    SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)(result_index++), type, &val, sizeof(T), &info);
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

private:
  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  
  bool free_;
  
  enum { NUMERIC_LEN = 21 };

  SQLHANDLE stmt_;
};

}

}

#endif /* MSSQL_RESULT_HPP */
