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

#ifndef SQLITE_STATEMENT_HPP
#define SQLITE_STATEMENT_HPP

#include "database/statement.hpp"

#include <mysql/mysql.h>

#include <string>

struct st_mysql_stmt;

namespace oos {

namespace mysql {

class mysql_database;

class mysql_statement : public statement
{
public:
  mysql_statement(mysql_database &db);
  virtual ~mysql_statement();

//  virtual result* execute(const std::string &sql);

  virtual bool step();
  virtual void prepare(const std::string &sql);
  virtual void reset(bool clear_bindings);
  
  int finalize();

  virtual int column_count() const;
  virtual const char* column_name(int i) const;

  virtual const char* column_text(int i, int &s) const;
  virtual int column_int(int i) const;
  virtual double column_double(int i) const;

  int column_type(int i) const;

  virtual int bind(int i, double value);
  virtual int bind(int i, int value);
  virtual int bind(int i, long value);
  virtual int bind(int i, unsigned int value);
  virtual int bind(int i, const char *value, int len);
  virtual int bind_null(int i);

  virtual database& db();
  virtual const database& db() const;

private:
  st_mysql_stmt *stmt_;
  mysql_database &db_;
};

}

}

#endif /* SQLITE_STATEMENT_HPP */
