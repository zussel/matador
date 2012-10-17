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
#include "database/database.hpp"
#include "database/session.hpp"

#include "tools/varchar.hpp"

#include "object/object_ptr.hpp"
#include "object/object.hpp"
#include "object/object_store.hpp"

#include <cstring>

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
//  o->write_to(this);
  o->serialize(*this);

  if (bind_id) {
    stmt_->bind(++column_, (int)o->id());
  }
}

void statement_serializer::read(statement_impl *stmt, object *o)
{
  stmt_ = stmt;

  column_ = 0;

//  o->read_from(this);
  o->deserialize(*this);
}

void statement_serializer::read(const char *id, char &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  // get column with name "id" from row,
  // convert value to char and assign value to x
  x = (char)stmt_->column_int(column_++);
}

void statement_serializer::read(const char *id, float &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = (float)stmt_->column_double(column_++);
}

void statement_serializer::read(const char *id, double &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_double(column_++);
}

void statement_serializer::read(const char *id, short &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_serializer::read(const char *id, int &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_serializer::read(const char *id, long &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_serializer::read(const char *id, unsigned short &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_serializer::read(const char *id, unsigned int &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_serializer::read(const char *id, unsigned long &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_serializer::read(const char *id, bool &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++) != 0;
}

void statement_serializer::read(const char *id, char* &x)
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
#ifdef WIN32
  strncpy_s(x, len, tmp, len);
#else
  strncpy(x, tmp, len);
#endif
  x[len] = '\0';
}

void statement_serializer::read(const char *id, std::string &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_text(column_++);
}

void statement_serializer::read(const char *id, varchar_base &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_text(column_++);
}

void statement_serializer::read(const char *id, object_base_ptr &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  long oid = 0;
  oid = stmt_->column_int(column_++);
  
  if (oid == 0) {
    return;
  }
  
  object_proxy *oproxy = stmt_->db().db()->ostore().find_proxy(oid);

  if (!oproxy) {
    oproxy = stmt_->db().db()->ostore().create_proxy(oid);
  }
  x.reset(oproxy->obj);
  //x.repoint(...);
  // set object id
}

bool statement_serializer::valid_column(const char *id, int i) const
{
  const char *name = stmt_->column_name(i);
  return strcmp(name, id) == 0;
}

void statement_serializer::write(const char*, char x)
{
  stmt_->bind(++column_, x);
}

void statement_serializer::write(const char*, float x)
{
  stmt_->bind(++column_, x);
}

void statement_serializer::write(const char*, double x)
{
  stmt_->bind(++column_, x);
}

void statement_serializer::write(const char*, short x)
{
  stmt_->bind(++column_, x);
}

void statement_serializer::write(const char*, int x)
{
  stmt_->bind(++column_, x);
}

void statement_serializer::write(const char*, long x)
{
  stmt_->bind(++column_, (int)x);
}

void statement_serializer::write(const char*, unsigned short x)
{
  stmt_->bind(++column_, x);
}

void statement_serializer::write(const char*, unsigned int x)
{
  stmt_->bind(++column_, x);
}

void statement_serializer::write(const char*, unsigned long x)
{
  stmt_->bind(++column_, (unsigned int)x);
}

void statement_serializer::write(const char*, bool x)
{
  stmt_->bind(++column_, x);
}

void statement_serializer::write(const char*, const char *x)
{
  stmt_->bind(++column_, x);
}

void statement_serializer::write(const char*, const std::string &x)
{
  stmt_->bind(++column_, x.c_str());
}

void statement_serializer::write(const char*, const varchar_base &x)
{
  stmt_->bind(++column_, x.c_str());
}

void statement_serializer::write(const char*, const object_base_ptr &x)
{
  if (x.ptr()) {
    stmt_->bind(++column_, (int)x.id());
  } else {
    stmt_->bind_null(++column_);
  }
}

}
