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

#ifndef MSSQL_STATEMENT_HPP
#define MSSQL_STATEMENT_HPP

#include "sql/statement_impl.hpp"
#include "sql/sql.hpp"

#include "mssql_exception.hpp"

#if defined(_MSC_VER)
#include <windows.h>
#endif

#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#include <string>
#include <vector>
#include <type_traits>

namespace oos {

namespace mssql {

class mssql_connection;

class mssql_statement : public detail::statement_impl
{
public:
  mssql_statement(mssql_connection &db, const sql &s);
  virtual ~mssql_statement();

  virtual void clear();
  virtual detail::result_impl* execute();
  virtual void reset();
  
//  virtual int column_count() const;
//  virtual const char* column_name(int i) const;

  static int type2int(data_type_t type);
  static int type2sql(data_type_t type);

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
  void bind_value(T val, size_t index)
  {
    value_t *v = new value_t;
    if (bind_null_) {
      v->data = NULL;
      v->len = SQL_NULL_DATA;
    } else {
      v->data = new char[sizeof(T)];
      *static_cast<T*>(v->data) = val;
    }
    host_data_.push_back(v);
    
    int ctype = mssql_statement::type2int(type_traits<T>::data_type());
    int type = mssql_statement::type2sql(type_traits<T>::data_type());
    SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, (SQLSMALLINT)ctype, (SQLSMALLINT)type, 0, 0, v->data, 0, &v->len);
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
  }
  void bind_value(const oos::date &d, size_t index);
  void bind_value(const oos::time &t, size_t index);
  void bind_value(unsigned long val, size_t index);
  void bind_value(const char *val, size_t size, size_t index);

  template < class T >
  void bind_null(size_t index)
  {
    value_t *v = new value_t;
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
    host_data_.push_back(v);

    int ctype = mssql_statement::type2int(type_traits<T>::data_type());
    int type = mssql_statement::type2sql(type_traits<T>::data_type());
    SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, (SQLSMALLINT)ctype, (SQLSMALLINT)type, 0, 0, NULL, 0, &v->len);
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind null parameter");
  }

private:
  void bind_null();
  void bind_value();

private:
  mssql_connection &db_;
  
  struct value_t {
    explicit value_t(bool fxd = true, SQLLEN l = 0) : fixed(fxd), len(l), data(0) {}
    ~value_t() { delete [] static_cast<char*>(data); }
    bool fixed;
    SQLLEN len;
    void *data;
  };
  std::vector<value_t*> host_data_;

  enum { NUMERIC_LEN = 21 };

  bool bind_null_ = false;

  SQLHANDLE stmt_;
};

}

}

#endif /* MSSQL_STATEMENT_HPP */
