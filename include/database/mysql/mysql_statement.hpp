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

#ifndef MYSQL_STATEMENT_HPP
#define MYSQL_STATEMENT_HPP

#include "database/statement.hpp"

#include "tools/enable_if.hpp"

#include <mysql/mysql.h>

#include <string>
#include <vector>
#include <type_traits>
#include <iostream>

struct st_mysql_stmt;

#ifdef WIN32
#define CPP11_TYPE_TRAITS_NS std::tr1
#else
#define CPP11_TYPE_TRAITS_NS std
#endif

namespace oos {

namespace mysql {

class mysql_database;

class mysql_statement : public statement
{
public:
  mysql_statement(mysql_database &db);
  virtual ~mysql_statement();

  virtual void execute();

  virtual bool fetch();
  virtual void prepare(const std::string &sql);
  virtual void reset(bool clear_bindings);
  
  virtual int column_count() const;
  virtual const char* column_name(int i) const;

  virtual void column(int i, bool &value);
  virtual void column(int i, char &value);
  virtual void column(int i, float &value);
  virtual void column(int i, double &value);
  virtual void column(int i, short &value);
  virtual void column(int i, int &value);
  virtual void column(int i, long &value);
  virtual void column(int i, unsigned char &value);
  virtual void column(int i, unsigned short &value);
  virtual void column(int i, unsigned int &value);
  virtual void column(int i, unsigned long &value);
  virtual void column(int i, char *value, int &len);
  virtual void column(int i, std::string &value);

  virtual int bind(int i, double value);
  virtual int bind(int i, int value);
  virtual int bind(int i, long value);
  virtual int bind(int i, unsigned int value);
  virtual int bind(int i, unsigned long value);
  virtual int bind(int i, const char *value, int len);
  virtual int bind(int i, const std::string &value);
  virtual int bind_null(int i);

  virtual database& db();
  virtual const database& db() const;

  template < class T >
  typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_integral<T>::value>::type prepare_result_column(int index)
  {
    result_[index].buffer_type = MYSQL_TYPE_LONG;
//    result_[index].buffer         = (void *) &int_val;
    result_[index].buffer         = new char[sizeof(T)];
    result_[index].buffer_length    = sizeof(T);
    result_[index].is_null         = 0;
    result_[index].length         = 0;
    std::cout << "creating result buffer of size " << sizeof(T) << "(address: " << result_[index].buffer << ")\n";
  }

  template < class T >
  typename oos::enable_if<CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value>::type prepare_result_column(int index)
  {
    std::cout << "creating result buffer of size " << sizeof(T) << "\n";
    result_[index].buffer_type = MYSQL_TYPE_DOUBLE;
//    result_[index].buffer         = (void *) &double_val;
    result_[index].buffer         = new char[sizeof(T)];
    result_[index].buffer_length    = sizeof(T);
    result_[index].is_null         = 0;
    result_[index].length         = 0;
  }

  template < class T >
  typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value &&
                          !CPP11_TYPE_TRAITS_NS::is_integral<T>::value>::type prepare_result_column(int index)
  {
    result_[index].buffer_type = MYSQL_TYPE_NULL;
    result_[index].buffer         = 0;
    result_[index].buffer_length    = 0;
    result_[index].is_null         = 0;
    result_[index].length         = 0;
  }

protected:
  virtual void on_result_field(const std::string &field, int index);
  virtual void on_host_field(const std::string &field, int index);

private:
  st_mysql_stmt *stmt_;
  mysql_database &db_;
  
  MYSQL_BIND *param_;
  MYSQL_BIND *result_;
  unsigned long *result_length_;
  
  std::vector<unsigned long> host_vector_;
  std::vector<unsigned long> result_vector_;
};

}

}

#endif /* MYSQL_STATEMENT_HPP */
