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

#include "matador/sql/statement_impl.hpp"
#include "matador/sql/sql.hpp"

#include "matador/db/mssql/mssql_exception.hpp"
#include "mssql_parameter_binder.hpp"

#if defined(_MSC_VER)
#include <windows.h>
#endif

#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#include <string>
#include <vector>
#include <type_traits>

namespace matador {

namespace mssql {

class mssql_connection;

class mssql_statement : public detail::statement_impl
{
public:
  mssql_statement(mssql_connection &db, const matador::sql &stmt);
  ~mssql_statement() override;

  void clear() override;
  detail::result_impl* execute() override;
  void reset() override;
  
//  virtual int column_count() const;
//  virtual const char* column_name(int i) const;

  static int type2int(data_type type);
  static int type2sql(data_type type);

protected:
  void serialize(const char*, char&) override;
  void serialize(const char*, short&) override;
  void serialize(const char*, int&) override;
  void serialize(const char*, long&) override;
  void serialize(const char*, unsigned char&) override;
  void serialize(const char*, unsigned short&) override;
  void serialize(const char*, unsigned int&) override;
  void serialize(const char*, unsigned long&) override;
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
  void bind_value(T val, size_t index)
  {
    auto *v = new value_t;
    if (bind_null_) {
      v->data = nullptr;
      v->len = SQL_NULL_DATA;
    } else {
      v->len = sizeof(T);
      v->data = new char[sizeof(T)];
      *reinterpret_cast<T*>(v->data) = val;
    }
    host_data_.push_back(v);
    
    auto ctype = (SQLSMALLINT)mssql_statement::type2int(data_type_traits<T>::type());
    auto type = (SQLSMALLINT)mssql_statement::type2sql(data_type_traits<T>::type());
    SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, ctype, type, 0, 0, v->data, 0, nullptr);
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
  }
  void bind_value(char c, size_t index);
  void bind_value(unsigned char c, size_t index);
  void bind_value(const matador::date &d, size_t index);
  void bind_value(const matador::time &t, size_t index);
  void bind_value(unsigned long val, size_t index);
  void bind_value(bool val, size_t index);
  void bind_value(const char *val, size_t s, size_t index);
  void bind_value(const std::string &str, size_t index);

  template < class T >
  void bind_null(size_t index)
  {
    auto *v = new value_t;
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
    host_data_.push_back(v);

    int ctype = mssql_statement::type2int(data_type_traits<T>::type());
    int type = mssql_statement::type2sql(data_type_traits<T>::type());
    SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, (SQLSMALLINT)ctype, (SQLSMALLINT)type,
                                     0, 0, nullptr, 0, nullptr);
    throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind null parameter");
  }

protected:
  detail::parameter_binder_impl *binder() const override;

private:
  void bind_null();
  void bind_value();

  void create_statement();

private:
  struct value_t {
    explicit value_t(SQLLEN l = 0) : len(l), data(nullptr) {}
    ~value_t() { delete [] data; }
    SQLLEN len;
    SQLLEN result_len = 0;
    char *data;
  };
  std::vector<value_t*> host_data_;
  std::unordered_map<PTR, value_t*> data_to_put_map_;

  enum { NUMERIC_LEN = 21 };

  bool bind_null_ = false;

  SQLHANDLE stmt_ = nullptr;
  SQLHANDLE db_ = nullptr;

  mssql_parameter_binder *binder_ = nullptr;
};

}

}

#endif /* MSSQL_STATEMENT_HPP */
