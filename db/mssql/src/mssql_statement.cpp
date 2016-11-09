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

#include "mssql_statement.hpp"
#include "mssql_connection.hpp"
#include "mssql_result.hpp"

#include "tools/varchar.hpp"
#include "tools/date.hpp"
#include "tools/time.hpp"
#include "tools/identifiable_holder.hpp"
#include "tools/basic_identifier.hpp"

#include <cstring>
#include <sstream>

namespace oos {

namespace mssql {

mssql_statement::mssql_statement(mssql_connection &db, const sql &s)
{
  if (!db.handle()) {
    throw_error("mssql", "no odbc connection established");
  }
  // create statement handle
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, db.handle(), &stmt_);
  throw_error(ret, SQL_HANDLE_DBC, db.handle(), "mssql", "error on creating sql statement");
  str(db.dialect()->prepare(s));

  ret = SQLPrepare(stmt_, (SQLCHAR*)str().c_str(), SQL_NTS);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, str());
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
}

void mssql_statement::clear()
{
  reset();
  SQLFreeHandle(SQL_HANDLE_STMT, stmt_);
}

detail::result_impl* mssql_statement::execute()
{
  SQLRETURN ret = SQLExecute(stmt_);

  // check result
  throw_error(ret, SQL_HANDLE_STMT, stmt_, str(), "error on query execute");

  return new mssql_result(stmt_);
}

void mssql_statement::serialize(const char *, char &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, short &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, int &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, long &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, unsigned char &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, unsigned short &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, unsigned int &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, unsigned long &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, bool &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, float &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, double &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, char *x, size_t s)
{
  bind_value(x, s, ++host_index);
}

void mssql_statement::serialize(const char *, std::string &x)
{
  bind_value(x.data(), x.size(), ++host_index);
}

void mssql_statement::serialize(const char *, oos::date &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, oos::time &x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::serialize(const char *, varchar_base &x)
{
  bind_value(x.data(), x.size(), ++host_index);
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

void mssql_statement::bind_value(const oos::date &d, size_t index)
{
  std::unique_ptr<value_t> v(new value_t(true, sizeof(SQL_DATE_STRUCT)));

  if (bind_null_) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->data = new char[v->len];
//    v->data = new SQL_DATE_STRUCT;

    SQL_DATE_STRUCT *ts = static_cast<SQL_DATE_STRUCT *>(v->data);

    ts->year = (SQLSMALLINT) d.year();
    ts->month = (SQLUSMALLINT) d.month();
    ts->day = (SQLUSMALLINT) d.day();
  }

  SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, SQL_C_TYPE_DATE, SQL_TIMESTAMP, sizeof(SQL_DATE_STRUCT), 0, v->data, 0, &v->len);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");

  host_data_.push_back(v.release());
}

void mssql_statement::bind_value(const oos::time &t, size_t index)
{
  std::unique_ptr<value_t> v(new value_t(true, SQL_NTS));

  if (bind_null_) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->data = new char[sizeof(SQL_TIMESTAMP_STRUCT)];

    SQL_TIMESTAMP_STRUCT *ts = static_cast<SQL_TIMESTAMP_STRUCT *>(v->data);

    ts->year = (SQLSMALLINT) t.year();
    ts->month = (SQLUSMALLINT) t.month();
    ts->day = (SQLUSMALLINT) t.day();
    ts->hour = (SQLUSMALLINT) t.hour();
    ts->minute = (SQLUSMALLINT) t.minute();
    ts->second = (SQLUSMALLINT) t.second();
    ts->fraction = (SQLUINTEGER) t.milli_second() * 1000 * 1000;
  }

  SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, sizeof(SQL_TIMESTAMP_STRUCT), 0, v->data, 0, &v->len);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");

  host_data_.push_back(v.release());
}

void mssql_statement::bind_value(unsigned long val, size_t index)
{
  value_t *v = new value_t(true, SQL_NTS);

  size_t size = 0;

  if (bind_null_) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {
    v->data = new char[NUMERIC_LEN];
#if defined(_MSC_VER)
    size = (int)_snprintf_s(static_cast<char*>(v->data), NUMERIC_LEN, NUMERIC_LEN, "%lu", val);
#else
    size = (size_t)snprintf(static_cast<char*>(v->data), NUMERIC_LEN, "%lu", val);
#endif
  }

  host_data_.push_back(v);

  SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, size, 0, v->data, 0, &v->len);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
}

void mssql_statement::bind_value(const char *val, size_t /*size*/, size_t index)
{
  size_t s = strlen(val);
  value_t *v = new value_t(false, SQL_NTS);

  if (bind_null_) {
    v->data = nullptr;
    v->len = SQL_NULL_DATA;
  } else {

    v->data = new char[s + 1];
#ifdef _MSC_VER
	strncpy_s((char *) v->data, s+1, val, s);
#else
	v->data = strncpy((char *)v->data, val, s);
#endif
    ((char *) v->data)[s++] = '\0';
  }

  host_data_.push_back(v);

  SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT)index, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, s, 0, v->data, 0, &v->len);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
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
      return SQL_C_UTINYINT;
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
      return SQL_BIGINT;
    case data_type::type_unsigned_char:
      return SQL_SMALLINT;
    case data_type::type_unsigned_short:
      return SQL_INTEGER;
    case data_type::type_unsigned_int:
      return SQL_BIGINT;
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
    case data_type::type_varchar:
      return SQL_VARCHAR;
    case data_type::type_text:
      return SQL_VARCHAR;
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

}

}
