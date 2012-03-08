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

#include "database/statement_serializer.hpp"
#include "database/statement.hpp"

#include "tools/varchar.hpp"

#include "object/object_ptr.hpp"
#include "object/object.hpp"

namespace oos {

statement_serializer::statement_serializer()
{}

statement_serializer::~statement_serializer()
{}

void statement_serializer::bind(statement_impl *stmt, object *o, bool bind_id)
{
  stmt_ = stmt;

  column_ = 0;
  // bind parameter
  o->write_to(this);

  if (bind_id) {
    stmt_->bind(++column_, (int)o->id());
  }
}

void statement_serializer::read(statement_impl *stmt, object *o)
{
  stmt_ = stmt;

  column_ = 0;

  o->read_from(this);
}

void statement_serializer::read_char(const char *id, char &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  // get column with name "id" from row,
  // convert value to char and assign value to x
  x = (char)stmt_->column_int(column_++);
}

void statement_serializer::read_float(const char *id, float &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = (float)stmt_->column_double(column_++);
}

void statement_serializer::read_double(const char *id, double &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_double(column_++);
}

void statement_serializer::read_int(const char *id, int &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_serializer::read_long(const char *id, long &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_serializer::read_unsigned(const char *id, unsigned &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_serializer::read_bool(const char *id, bool &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++) != 0;
}

void statement_serializer::read_charptr(const char *id, char* &x)
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

void statement_serializer::read_string(const char *id, std::string &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_text(column_++);
}

void statement_serializer::read_varchar(const char *id, varchar_base &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_text(column_++);
}

void statement_serializer::read_object(const char *id, object_base_ptr &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  //x.repoint(...);
  // set object id
}

bool statement_serializer::valid_column(const char *id, int i) const
{
  const char *name = stmt_->column_name(i);
  return strcmp(name, id) == 0;
}

void statement_serializer::write_char(const char *id, char x)
{
//  std::cout << "bind value for column [" << id << "] (value: " << x << ")\n";
  stmt_->bind(++column_, x);
}

void statement_serializer::write_float(const char *id, float x)
{
//  std::cout << "bind value for column [" << id << "] (value: " << x << ")\n";
  stmt_->bind(++column_, x);
}

void statement_serializer::write_double(const char *id, double x)
{
//  std::cout << "bind value for column [" << id << "] (value: " << x << ")\n";
  stmt_->bind(++column_, x);
}

void statement_serializer::write_int(const char *id, int x)
{
//  std::cout << "bind value for column [" << id << "] (value: " << x << ")\n";
  stmt_->bind(++column_, x);
}

void statement_serializer::write_long(const char *id, long x)
{
//  std::cout << "bind value for column [" << id << "] (value: " << x << ")\n";
  stmt_->bind(++column_, (int)x);
}

void statement_serializer::write_unsigned(const char *id, unsigned x)
{
//  std::cout << "bind value for column [" << id << "] (value: " << x << ")\n";
  stmt_->bind(++column_, x);
}

void statement_serializer::write_bool(const char *id, bool x)
{
//  std::cout << "bind value for column [" << id << "] (value: " << x << ")\n";
  stmt_->bind(++column_, x);
}

void statement_serializer::write_charptr(const char *id, const char *x)
{
//  std::cout << "bind value for column [" << id << "] (value: " << x << ")\n";
  stmt_->bind(++column_, x);
}

void statement_serializer::write_string(const char *id, const std::string &x)
{
//  std::cout << "bind value for column [" << id << "] (value: " << x << ")\n";
  stmt_->bind(++column_, x.c_str());
}

void statement_serializer::write_varchar(const char *id, const varchar_base &x)
{
//  std::cout << "bind value for column [" << id << "] (value: " << x.str() << ")\n";
  stmt_->bind(++column_, x.c_str());
}

void statement_serializer::write_object(const char *id, const object_base_ptr &x)
{
//  std::cout << "bind value for column [" << id << "] (value: " << x << ")\n";
  if (x.ptr()) {
    stmt_->bind(++column_, (int)x.id());
  } else {
    stmt_->bind_null(++column_);
  }
}

}
