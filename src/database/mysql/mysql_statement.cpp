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

#include "database/mysql/mysql_statement.hpp"
#include "database/mysql/mysql_database.hpp"
#include "database/mysql/mysql_exception.hpp"

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstring>

#include <mysql/mysql.h>

namespace oos {

namespace mysql {

void throw_error(int ec, st_mysql *db, const std::string &source, const std::string &sql = "")
{
  if (ec == 0) {
    return;
  }
  std::stringstream msg;
  msg << source << ": " << mysql_error(db) << "(" << sql << ")";
  throw mysql_exception(msg.str()); 
}

void throw_stmt_error(int ec, MYSQL_STMT *stmt, const std::string &source, const std::string &sql = "")
{
  if (ec == 0) {
    return;
  }
  std::stringstream msg;
  msg << source << ": " << mysql_stmt_error(stmt) << "(" << sql << ")";
  throw mysql_exception(msg.str()); 
}

mysql_statement::mysql_statement(mysql_database &db)
  : stmt_(0)
  , db_(db)
  , param_(0)
  , result_(0)
{
}

mysql_statement::~mysql_statement()
{
  if (param_) {
    delete [] param_;
  }
  if (result_) {
    // free data
    for (unsigned long i = 0; i < result_vector_.size(); ++i) {
      std::cout << "deleting result buffer of size " << result_[i].buffer_length << "\n";
//      delete [] static_cast<char*>(result_[i].buffer);
    }
    delete [] result_;
  }
}

void mysql_statement::execute()
{
  std::cout << "binding result\n";
  int ret = mysql_stmt_bind_result(stmt_, result_);
  throw_stmt_error(ret, stmt_, "mysql_stmt_execute");
  
  std::cout << "executing statement\n";
  ret = mysql_stmt_execute(stmt_);
  throw_stmt_error(ret, stmt_, "mysql_stmt_execute");
  
  std::cout << "storing result\n";
  ret = mysql_stmt_store_result(stmt_);
  throw_stmt_error(ret, stmt_, "mysql_stmt_execute");
}

bool mysql_statement::fetch()
{

//  std::cerr << "executing " << sql() << "\n";

  int ret = mysql_stmt_fetch(stmt_);
  if (ret == 0) {
    // retrieved new row
    // create row object
    return true;
  } else if (ret == MYSQL_NO_DATA) {
    return false;
  } else if (ret == MYSQL_DATA_TRUNCATED) {
    std::stringstream msg;
    msg << "mysql_stmt_fetch: data truncated (bind.error: " << 0 << ")";
    throw mysql_exception(msg.str());
  } else {
    // error, throw exception
    throw_stmt_error(ret, stmt_, "mysql_stmt_fetch");
  }
  return false;
}

void mysql_statement::prepare(const std::string &sql)
{
  host_vector_.clear();
  result_vector_.clear();
  // set new sql statement
  statement::prepare(sql);
  
//  std::cout << "prepared statement: [" << this->sql() << "]\n";
  // create statement
  stmt_ = mysql_stmt_init(db_());
  // prepare mysql statement
  int ret = mysql_stmt_prepare(stmt_, this->sql().c_str(), this->sql().size());
  throw_error(ret, db_(), "mysql_stmt_prepare", sql);
  // create result/binding param array
  
  param_ = new MYSQL_BIND[host_vector_.size()];
  memset(param_, 0, host_vector_.size() * sizeof(MYSQL_BIND));
  result_ = new MYSQL_BIND[result_vector_.size()];
  memset(result_, 0, result_vector_.size() * sizeof(MYSQL_BIND));
  
  
//  result_length_ = new unsigned long[results];
  for (unsigned long i = 0; i < (unsigned long)result_vector_.size(); ++i) {
//    result_length_[i] = 0;
//    result_[i].length = &result_length_[i];
  }
}

void mysql_statement::reset(bool)
{
  mysql_stmt_reset(stmt_);
}

int mysql_statement::column_count() const
{
  return mysql_stmt_field_count(stmt_);
}

const char* mysql_statement::column_name(int) const
{
  throw mysql_exception("unsupported feature");
}

template < class T >
void prepare_result(MYSQL_BIND &result, T &val, enum_field_types type)
{
  memset(&result, 0, sizeof(result));
  result.buffer_type = type;
	result.buffer         = (void *) &val;
	result.buffer_length    = 0;
	result.is_null         = 0;
	result.length         = 0;
}

void prepare_result(MYSQL_BIND &result, char *&val, int len, unsigned long &data_len)
{
  memset(&result, 0, sizeof(result));
  result.buffer_type= MYSQL_TYPE_VAR_STRING;
  result.buffer = val;
  result.buffer_length = len;
  result.is_null = 0;
  result.length = &data_len;
}

void mysql_statement::column(int i, bool &value)
{
  prepare_result(result_[i], value, MYSQL_TYPE_LONG);
  mysql_stmt_fetch_column(stmt_, &result_[i], i, 0);
}

void mysql_statement::column(int i, char &value)
{
  value = *static_cast<char*>(result_[i].buffer);
  std::cout << "buffer length: [" << result_[i].buffer_length << "]\n";
}

void mysql_statement::column(int i, float &value)
{
  prepare_result(result_[i], value, MYSQL_TYPE_LONG);
  mysql_stmt_fetch_column(stmt_, &result_[i], i, 0);
}

void mysql_statement::column(int i, double &value)
{
  prepare_result(result_[i], value, MYSQL_TYPE_LONG);
  mysql_stmt_fetch_column(stmt_, &result_[i], i, 0);
}

void mysql_statement::column(int i, short &value)
{
  value = *static_cast<short*>(result_[i].buffer);
  std::cout << "buffer length: [" << result_[i].buffer_length << "]\n";
}

void mysql_statement::column(int i, int &value)
{
//  value = int_val;
  value = *static_cast<int*>(result_[i].buffer);
  std::cout << "buffer length: [" << result_[i].buffer_length << "]\n";
}

void mysql_statement::column(int i, long &value)
{
  value = *static_cast<long*>(result_[i].buffer);
  std::cout << "buffer length: [" << result_[i].buffer_length << "]\n";
}

void mysql_statement::column(int i, unsigned char &value)
{
  value = *static_cast<unsigned char*>(result_[i].buffer);
  std::cout << "buffer length: [" << result_[i].buffer_length << "]\n";
}

void mysql_statement::column(int i, unsigned short &value)
{
  value = *static_cast<unsigned short*>(result_[i].buffer);
  std::cout << "buffer length: [" << result_[i].buffer_length << "]\n";
}

void mysql_statement::column(int i, unsigned int &value)
{
  value = *static_cast<unsigned int*>(result_[i].buffer);
  std::cout << "buffer length: [" << result_[i].buffer_length << "]\n";
}

void mysql_statement::column(int i, unsigned long &value)
{
  value = *static_cast<unsigned long*>(result_[i].buffer);
  std::cout << "buffer length: [" << result_[i].buffer_length << "]\n";
}

void mysql_statement::column(int i, char *value, int &len)
{
  unsigned long data_length;
  prepare_result(result_[i], value, len, data_length);
}

void mysql_statement::column(int i, std::string &value)
{
  unsigned long data_length;
  char *buf = new char[value.size()];
  prepare_result(result_[i], buf, value.size(), data_length);
  value.assign(buf, data_length);
}

int mysql_statement::bind(int i, double value)
{
  prepare_result(param_[i], value, MYSQL_TYPE_DOUBLE);
  return 0;
}

int mysql_statement::bind(int i, int value)
{
  prepare_result(param_[i], value, MYSQL_TYPE_LONG);
  return 0;
}

int mysql_statement::bind(int i, long value)
{
  prepare_result(param_[i], value, MYSQL_TYPE_LONG);
  return 0;
}

int mysql_statement::bind(int i, unsigned int value)
{
  prepare_result(param_[i], value, MYSQL_TYPE_LONG);
  return 0;
}

int mysql_statement::bind(int i, unsigned long value)
{
  prepare_result(param_[i], value, MYSQL_TYPE_LONG);
  return 0;
}

int mysql_statement::bind(int i, const char *value, int len)
{
  param_[i].buffer_type = MYSQL_TYPE_STRING;
  param_[i].buffer = (char *)value;
  param_[i].buffer_length = len;
  param_[i].is_null = 0;
  param_[i].length = 0/*&str_length*/;
  return 0;
}

int mysql_statement::bind(int i, const std::string &value)
{
  param_[i].buffer_type = MYSQL_TYPE_STRING;
  param_[i].buffer = (char *)value.data();
  param_[i].buffer_length = value.size();
  param_[i].is_null = 0;
  param_[i].length = 0/*&str_length*/;
  return 0;
}

int mysql_statement::bind_null(int i)
{
  param_[i].buffer = 0;
  param_[i].buffer_length = 0;
  param_[i].length = 0;
  param_[i].buffer_type = MYSQL_TYPE_NULL;
  param_[i].is_null = 0;
  return 0;
}

database& mysql_statement::db()
{
  return db_;
}

const database& mysql_statement::db() const
{
  return db_;
}

void mysql_statement::on_result_field(const std::string &field, int index)
{
  result_vector_.push_back(0);  
  std::cout << "result token: " << field << " (index: " << index << ")\n";
}

void mysql_statement::on_host_field(const std::string &field, int index)
{
  host_vector_.push_back(0);  
  std::cout << "host token: " << field << " (index: " << index << ")\n";
}

}

}
