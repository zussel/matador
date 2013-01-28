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

#include <string>

struct sqlite3_stmt;

namespace oos {

namespace sqlite {

class sqlite_database;

class sqlite_statement : public statement
{
public:
  sqlite_statement(sqlite_database &db);
  virtual ~sqlite_statement();

//  virtual result* execute(const std::string &sql);

  virtual void execute() {}
  virtual bool step();
  virtual void prepare(const std::string &sql);
  virtual void reset(bool clear_bindings);
  
  int finalize();

  virtual int column_count() const;
  virtual const char* column_name(int i) const;

  virtual void column(int i, bool &value);
  virtual void column(int i, char &value);
  virtual void column(int i, float &value);
  virtual void column(int i, double &value);
  virtual void column(int i, unsigned char &value);
  virtual void column(int i, unsigned short &value);
  virtual void column(int i, unsigned int &value);
  virtual void column(int i, unsigned long &value);
  virtual void column(int i, short &value);
  virtual void column(int i, int &value);
  virtual void column(int i, long &value);
  virtual void column(int i, char *value, int &len);
  virtual void column(int i, std::string &value);

  int column_type(int i) const;

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

private:
  sqlite3_stmt *stmt_;
  sqlite_database &db_;
};

}

}

#endif /* SQLITE_STATEMENT_HPP */
