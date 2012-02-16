/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY {} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "database/reader.hpp"
#include "database/database.hpp"
#include "database/statement_helper.hpp"

#include "object/object.hpp"

#include "tools/varchar.hpp"

#include <cstring>

namespace oos {

reader::reader(database &db, object *o)
  : db_(db)
  , column_(0)
{
  stmt_ = db_.impl_->find_statement(std::string(o->object_type()) + "_SELECT");
  if (!stmt_) {
    // throw database_error("couldn't find statement");
    // or create statement
  }
}

reader::~reader()
{}

bool reader::read()
{
  column_ = 0;
  return stmt_->step();
}

void reader::read_char(const char *id, char &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  // get column with name "id" from row,
  // convert value to char and assign value to x
  x = (char)stmt_->column_int(column_++);
}

void reader::read_float(const char *id, float &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = (float)stmt_->column_double(column_++);
}

void reader::read_double(const char *id, double &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_double(column_++);
}

void reader::read_int(const char *id, int &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void reader::read_long(const char *id, long &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void reader::read_unsigned(const char *id, unsigned &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void reader::read_bool(const char *id, bool &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++) != 0;
}

void reader::read_charptr(const char *id, char* &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  if (x) {
    delete [] x;
  }
  const char *tmp = stmt_->column_text(column_++);
  size_t len = strlen(tmp);
  x = new char[len + 1];
  strncpy(x, tmp, len);
  x[len] = '\0';
}

void reader::read_string(const char *id, std::string &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_text(column_++);
}

void reader::read_varchar(const char *id, varchar_base &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_text(column_++);
}

void reader::read_object(const char *id, object_base_ptr &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  //x.repoint(...);
  // set object id
}

bool reader::valid_column(const char *id, int i) const
{
  const char *name = stmt_->column_name(i);
  return strcmp(name, id) == 0;
}

}
