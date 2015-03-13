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
#include "database/sql.hpp"

#ifdef WIN32
#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <string>
#include <vector>
#include <type_traits>
#include <object/primary_key.hpp>

namespace oos {

class database;

namespace mysql {

class mysql_database;

class mysql_statement : public statement
{
public:
  explicit mysql_statement(mysql_database &db);
  mysql_statement(mysql_database &db, const sql &s);
  virtual ~mysql_statement();

  virtual void clear();
  virtual result* execute();
  virtual void prepare(const sql &s);
  virtual void reset();
  
//  virtual int column_count() const;
//  virtual const char* column_name(int i) const;

  virtual database& db();
  virtual const database& db() const;

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
	virtual void write(const char *id, const object_base_ptr &x);
  virtual void write(const char *id, const object_container &x);
  virtual void write(const char *id, const primary_key_base &x);

//  virtual void prepare_result_column(const sql::field_ptr &fptr);

private:
  template < class T >
  void bind_value(MYSQL_BIND &bind, enum_field_types type, T value, int /*index*/)
  {
    if (bind.buffer == 0) {
      // allocating memory
      bind.buffer = new char[sizeof(T)];
    }
    *static_cast<T*>(bind.buffer) = value;
    bind.buffer_type = type;
    bind.is_null = 0;
  }
  void bind_value(MYSQL_BIND &bind, enum_field_types type, int index);
  void bind_value(MYSQL_BIND &bind, enum_field_types type, const char *value, int size, int index);
  void bind_value(MYSQL_BIND &bind, enum_field_types type, const object_base_ptr &value, int index);

  static enum_field_types type_enum(data_type_t type);

private:
  mysql_database &db_;
  int result_size;
  int host_size;
  std::vector<unsigned long> length_vector;
  MYSQL_STMT *stmt;
  MYSQL_BIND *host_array;
};

}

}

#endif /* MYSQL_STATEMENT_HPP */
