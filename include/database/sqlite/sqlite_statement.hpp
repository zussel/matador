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

#include "object/object_atomizer.hpp"

#include <string>
#include <vector>

struct sqlite3_stmt;

namespace oos {

class varchar_base;

namespace sqlite {

class sqlite_database;

class sqlite_statement : public statement, public object_writer
{
public:
  sqlite_statement(sqlite_database &db);
  virtual ~sqlite_statement();

//  virtual result* execute(const std::string &sql);

  virtual result* execute();
  virtual void prepare(const std::string &sql);
  virtual void reset();
  virtual void bind(object_atomizable *o);

  int finalize();

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

private:
  sqlite_database &db_;
  sqlite3_stmt *stmt_;
  std::string sqlstr;
  int result_size;
  int result_index;
  int host_size;
  int host_index;
  std::vector<bool> host_data;
};

}

}

#endif /* SQLITE_STATEMENT_HPP */
