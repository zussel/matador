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

#include "database/statement.hpp"
#include "database/sql.hpp"

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
#include "identifier.hpp"

namespace oos {

class database;

namespace mssql {

class mssql_database;

class mssql_statement : public statement
{
public:
  explicit mssql_statement(mssql_database &db);
  mssql_statement(mssql_database &db, const sql &s);
  virtual ~mssql_statement();

  virtual void clear();
  virtual result* execute();
  virtual void prepare(const sql &s);
  virtual void reset();
  
//  virtual int column_count() const;
//  virtual const char* column_name(int i) const;

  virtual database& db();
  virtual const database& db() const;

  static int type2int(data_type_t type);
  static int type2sql(data_type_t type);

protected:
  virtual void write(const char *id, char x);
  virtual void write(const char *id, short x);
  virtual void write(const char *id, int x);
  virtual void write(const char *id, long x);
  virtual void write(const char *id, unsigned char x);
  virtual void write(const char *id, unsigned short x);
  virtual void write(const char *id, unsigned int x);
  virtual void write(const char *id, unsigned long x);
  virtual void write(const char *id, float x);
  virtual void write(const char *id, double x);
  virtual void write(const char *id, bool x);
	virtual void write(const char *id, const char *x, int s);
  virtual void write(const char *id, const varchar_base &x);
  virtual void write(const char *id, const std::string &x);
  virtual void write(const char *id, const oos::date &x);
  virtual void write(const char *id, const oos::time &x);
	virtual void write(const char *id, const object_base_ptr &x);
  virtual void write(const char *id, const object_container &x);
  virtual void write(const char *id, const primary_key_base &x);

  template < class T >
  void bind_value(T val, int index)
  {
    value_t *v = new value_t;
    v->data = new char[sizeof(T)];
    *static_cast<T*>(v->data) = val;
    host_data_.push_back(v);
    
    int ctype = mssql_statement::type2int(type_traits<T>::data_type());
    int type = mssql_statement::type2sql(type_traits<T>::data_type());
    SQLRETURN ret = SQLBindParameter(stmt_, index, SQL_PARAM_INPUT, ctype, type, 0, 0, v->data, 0, &v->len);
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
  }
  void bind_value(const oos::date &d, int index);
  void bind_value(const oos::time &t, int index);
  void bind_value(unsigned long val, int index);
  void bind_value(const char *val, int size, int index);

private:
  mssql_database &db_;
  
  struct value_t {
    explicit value_t(bool fxd = true, SQLLEN l = 0) : fixed(fxd), len(l), data(0) {}
    ~value_t() { delete [] static_cast<char*>(data); }
    bool fixed;
    SQLLEN len;
    void *data;
  };
  std::vector<value_t*> host_data_;

  enum { NUMERIC_LEN = 21 };

  SQLHANDLE stmt_;
};

}

}

#endif /* MSSQL_STATEMENT_HPP */
