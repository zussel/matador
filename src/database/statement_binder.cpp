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

#include "database/statement_binder.hpp"
#include "database/statement.hpp"

#include "tools/varchar.hpp"

#include "object/object_ptr.hpp"
#include "object/object.hpp"

namespace oos {

statement_binder::statement_binder()
  : stmt_(0)
  , column_(0)
{}

statement_binder::~statement_binder()
{}

void statement_binder::bind(statement_impl *stmt, object *o, bool bind_id)
{
  stmt_ = stmt;

  std::cerr << "start binding object values of object " << *o << "\n";
  column_ = 0;
  // bind parameter
  o->write_to(this);

  if (bind_id) {
    stmt_->bind(++column_, (int)o->id());
  }

  std::cout.flush();
}

void statement_binder::write_char(const char *id, char x)
{
  std::cout << "bind value for column [" << id << "]\n";
  stmt_->bind(++column_, x);
}

void statement_binder::write_float(const char *id, float x)
{
  std::cout << "bind value for column [" << id << "]\n";
  stmt_->bind(++column_, x);
}

void statement_binder::write_double(const char *id, double x)
{
  std::cout << "bind value for column [" << id << "]\n";
  stmt_->bind(++column_, x);
}

void statement_binder::write_int(const char *id, int x)
{
  std::cout << "bind value for column [" << id << "]\n";
  stmt_->bind(++column_, x);
}

void statement_binder::write_long(const char *id, long x)
{
  std::cout << "bind value for column [" << id << "]\n";
  stmt_->bind(++column_, (int)x);
}

void statement_binder::write_unsigned(const char *id, unsigned x)
{
  std::cout << "bind value for column [" << id << "]\n";
  stmt_->bind(++column_, x);
}

void statement_binder::write_bool(const char *id, bool x)
{
  std::cout << "bind value for column [" << id << "]\n";
  stmt_->bind(++column_, x);
}

void statement_binder::write_charptr(const char *id, const char *x)
{
  std::cout << "bind value for column [" << id << "]\n";
  stmt_->bind(++column_, x);
}

void statement_binder::write_string(const char *id, const std::string &x)
{
  std::cout << "bind value for column [" << id << "]\n";
  stmt_->bind(++column_, x.c_str());
}

void statement_binder::write_varchar(const char *id, const varchar_base &x)
{
  std::cout << "bind value for column [" << id << "]\n";
  stmt_->bind(++column_, x.str().c_str());
}

void statement_binder::write_object(const char *id, const object_base_ptr &x)
{
  std::cout << "bind value for column [" << id << "]\n";
  if (x.ptr()) {
    stmt_->bind(++column_, (int)x.id());
  } else {
    stmt_->bind_null(++column_);
  }
}

}
