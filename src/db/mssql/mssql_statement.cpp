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

#include "matador/db/mssql/mssql_statement.hpp"
#include "matador/db/mssql/mssql_connection.hpp"
#include "matador/db/mssql/mssql_result.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/identifiable_holder.hpp"
#include "matador/utils/basic_identifier.hpp"

#include <cstring>
#include <sstream>

namespace matador {

namespace mssql {

mssql_statement::mssql_statement(mssql_connection &db, const matador::sql &stmt)
  : statement_impl(db.dialect(), stmt)
  , db_(db.handle())
{
  if (!db_) {
    throw_error("mssql", "no odbc connection established");
  }

  create_statement();
}

mssql_statement::~mssql_statement()
{
  clear();
}

void mssql_statement::reset()
{
  while (!host_data_.empty()) {
    delete host_data_.back();
    host_data_.pop_back();
  }
  data_to_put_map_.clear();
}

void mssql_statement::clear()
{
  reset();
  SQLFreeHandle(SQL_HANDLE_STMT, stmt_);
}

detail::result_impl* mssql_statement::execute()
{
  SQLRETURN ret = SQLExecute(stmt_);
  // check if data is needed
  if (ret == SQL_NEED_DATA) {
    // put needed data from host_data

    // get first data to put
    PTR pid{ nullptr };
    ret = SQLParamData(stmt_, &pid);
    // get data from map
    auto it = data_to_put_map_.find(pid);
    if (it == data_to_put_map_.end()) {
      throw_error("mssql", "couldn't find data to put");
    }
    value_t *val = it->second;
    // Todo
    // put data as long it is requested
    while (ret == SQL_NEED_DATA) {
      while (val->len > 256) {
        ret = SQLPutData(stmt_, val->data, 256);
        val->len -= 256;
        val->data += 256;
      }
      ret = SQLPutData(stmt_, val->data, val->len);
      ret = SQLParamData(stmt_, &pid);
      if (!is_success(ret) && ret != SQL_NEED_DATA) {
        // error
        throw_error(ret, SQL_HANDLE_STMT, stmt_, "execute->SQLParamData", str());
      } else if (ret == SQL_NEED_DATA) {
        // determine next column data pointer
        it = data_to_put_map_.find(pid);
        if (it == data_to_put_map_.end()) {
          throw_error("mssql", "couldn't find data to put");
        }
        val = it->second;
      }
    }
  } else {
    // check result
    throw_error(ret, SQL_HANDLE_STMT, stmt_, str(), "error on query execute");
  }

  auto *res = new mssql_result(stmt_);

  create_statement();

  return res;
}

void mssql_statement::serialize(const char *id, char &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, short &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, int &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, long &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, unsigned char &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, unsigned short &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, unsigned int &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, unsigned long &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, bool &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, float &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, double &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, char *x, size_t s)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, s, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, std::string &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, matador::date &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, matador::time &x)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x, ++host_index);
  }
}

void mssql_statement::serialize(const char *id, std::string &x, size_t)
{
  if (host_var_.empty() || host_var_ == id) {
    bind_value(x.c_str(), x.capacity(), ++host_index);
  }
}

void mssql_statement::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key()->serialize(id, *this);
  } else {
    std::unique_ptr<basic_identifier> bid(x.create_identifier());
    bind_null();
    bid->serialize(id, *this);
    bind_value();
  }
}

void mssql_statement::serialize(const char *id, basic_identifier &x)
{
  x.serialize(id, *this);
}

void mssql_statement::bind_value(char c, size_t index)
{
  auto *v = new value_t;
  if (bind_null_) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->len = sizeof(char);
    v->data = new char[1];
    v->data[0] = c;
  }
  host_data_.push_back(v);

  auto ctype = (SQLUSMALLINT)mssql_statement::type2int(data_type_traits<char>::type());
  auto type = (SQLUSMALLINT)mssql_statement::type2sql(data_type_traits<char>::type());
  SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, ctype, type, 1, 0, v->data, v->len, &v->len);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
}

void mssql_statement::bind_value(unsigned char c, size_t index)
{
  auto *v = new value_t;
  if (bind_null_) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->len = sizeof(unsigned char);
    v->data = new char[1];
    v->data[0] = c;
  }
  host_data_.push_back(v);

  auto ctype = (SQLUSMALLINT)mssql_statement::type2int(data_type_traits<unsigned char>::type());
  auto type = (SQLUSMALLINT)mssql_statement::type2sql(data_type_traits<unsigned char>::type());
  SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, ctype, type, 1, 0, v->data, v->len, &v->len);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
}

void mssql_statement::bind_value(bool val, size_t index)
{
  auto *v = new value_t;
  if (bind_null_) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  }
  else {
    v->data = new char[sizeof(unsigned short)];
    *reinterpret_cast<unsigned short*>(v->data) = (unsigned short)val;
  }
  host_data_.push_back(v);

  auto ctype = (SQLSMALLINT)mssql_statement::type2int(data_type_traits<bool>::type());
  auto type = (SQLSMALLINT)mssql_statement::type2sql(data_type_traits<bool>::type());
  SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, ctype, type, 0, 0, v->data, 0, nullptr);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
}

void mssql_statement::bind_value(const matador::date &d, size_t index)
{
  std::unique_ptr<value_t> v(new value_t(true));

  if (bind_null_) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->data = new char[sizeof(SQL_DATE_STRUCT)];
    v->len = sizeof(SQL_DATE_STRUCT);

    auto *ts = reinterpret_cast<SQL_DATE_STRUCT *>(v->data);

    ts->year = (SQLSMALLINT) d.year();
    ts->month = (SQLUSMALLINT) d.month();
    ts->day = (SQLUSMALLINT) d.day();
  }

  SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, SQL_C_TYPE_DATE, SQL_TYPE_TIMESTAMP, 10, 0, v->data, 0, &v->len);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");

  host_data_.push_back(v.release());
}

void mssql_statement::bind_value(const matador::time &t, size_t index)
{
  std::unique_ptr<value_t> v(new value_t(true));

  if (bind_null_) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->data = new char[sizeof(SQL_TIMESTAMP_STRUCT)];
    v->len = sizeof(SQL_TIMESTAMP_STRUCT);

    auto *ts = reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(v->data);

    ts->year = (SQLSMALLINT) t.year();
    ts->month = (SQLUSMALLINT) t.month();
    ts->day = (SQLUSMALLINT) t.day();
    ts->hour = (SQLUSMALLINT) t.hour();
    ts->minute = (SQLUSMALLINT) t.minute();
    ts->second = (SQLUSMALLINT) t.second();
    ts->fraction = (SQLUINTEGER) t.milli_second() * 1000 * 1000;
  }

  SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, 23, 3, v->data, 0, &v->len);

  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");

  host_data_.push_back(v.release());
}

void mssql_statement::bind_value(unsigned long val, size_t index)
{
  auto *v = new value_t;
  if (bind_null_) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
	  v->len = sizeof(unsigned long);
    v->data = new char[v->len];
    *reinterpret_cast<unsigned long*>(v->data) = val;

  }
  host_data_.push_back(v);

  SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_BIGINT, 0, 0, v->data, 0, &v->len);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
}

void mssql_statement::bind_value(const char *val, size_t size, size_t index)
{
  auto *v = new value_t(strlen(val));

  if (bind_null_) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {

    v->data = new char[size];
#ifdef _MSC_VER
	strcpy_s((char *) v->data, size, val);
#else
	v->data = strcpy((char *)v->data, val);
#endif
  }

  host_data_.push_back(v);

  SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, size, 0, v->data, v->len, nullptr);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
}

void mssql_statement::bind_value(const std::string &str, size_t index)
{
  size_t s = str.size();
  auto *v = new value_t(SQL_NTS);

  if (bind_null_) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {

    v->data = new char[s + 1];
    v->len = s;
#ifdef _MSC_VER
    strncpy_s(v->data, s + 1, str.c_str(), s);
#else
    v->data = strncpy(v->data, str.c_str(), s);
#endif
    (v->data)[s++] = '\0';
  }

  host_data_.push_back(v);
  data_to_put_map_.insert(std::make_pair(v->data, v));

  SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_LONGVARCHAR, v->len, 0, v->data, v->len, &v->result_len);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
  v->result_len = SQL_LEN_DATA_AT_EXEC((SQLLEN)v->len);
}

int mssql_statement::type2int(data_type type)
{
  switch(type) {
    case data_type::type_char:
      return SQL_C_CHAR;
    case data_type::type_short:
      return SQL_C_SSHORT;
    case data_type::type_int:
      return SQL_C_SLONG;
    case data_type::type_long:
      return SQL_C_SLONG;
    case data_type::type_unsigned_char:
      return SQL_C_CHAR;
    case data_type::type_unsigned_short:
      return SQL_C_USHORT;
    case data_type::type_unsigned_int:
      return SQL_C_ULONG;
    case data_type::type_unsigned_long:
      return SQL_C_ULONG;
    case data_type::type_bool:
      return SQL_C_USHORT;
    case data_type::type_float:
      return SQL_C_FLOAT;
    case data_type::type_double:
      return SQL_C_DOUBLE;
    case data_type::type_char_pointer:
      return SQL_C_CHAR;
    case data_type::type_varchar:
      return SQL_C_CHAR;
    case data_type::type_text:
      return SQL_C_CHAR;
    case data_type::type_date:
      return SQL_C_TYPE_DATE;
    case data_type::type_time:
      return SQL_C_TYPE_TIMESTAMP;
    default:
      {
        throw std::logic_error("mssql statement: unknown type");
      }
    }
}

int mssql_statement::type2sql(data_type type)
{
  switch(type) {
    case data_type::type_char:
      return SQL_CHAR;
    case data_type::type_short:
      return SQL_SMALLINT;
    case data_type::type_int:
      return SQL_INTEGER;
    case data_type::type_long:
      return SQL_INTEGER;
    case data_type::type_unsigned_char:
      return SQL_CHAR;
    case data_type::type_unsigned_short:
      return SQL_INTEGER;
    case data_type::type_unsigned_int:
      return SQL_INTEGER;
    case data_type::type_unsigned_long:
      return SQL_NUMERIC;
    case data_type::type_bool:
      return SQL_INTEGER;
    case data_type::type_float:
      return SQL_FLOAT;
    case data_type::type_double:
      return SQL_DOUBLE;
    case data_type::type_char_pointer:
      return SQL_VARCHAR;
      //return SQL_LONGVARCHAR;
    case data_type::type_varchar:
      return SQL_VARCHAR;
      //return SQL_LONGVARCHAR;
    case data_type::type_text:
      return SQL_LONGVARCHAR;
    case data_type::type_date:
      return SQL_TIMESTAMP;
    case data_type::type_time:
      return SQL_TYPE_TIMESTAMP;
    default:
      {
        throw std::logic_error("mssql statement: unknown type");
      }
    }
}

void mssql_statement::bind_null()
{
  bind_null_ = true;
}

void mssql_statement::bind_value()
{
  bind_null_ = false;
}

void mssql_statement::create_statement()
{
  // create statement handle
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, db_, &stmt_);
  throw_error(ret, SQL_HANDLE_DBC, db_, "mssql", "error on creating sql statement");

  ret = SQLPrepare(stmt_, (SQLCHAR*)str().c_str(), SQL_NTS);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, str());
}

}

}
