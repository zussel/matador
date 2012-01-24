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

#include "database/statement_helper.hpp"

#include "tools/varchar.hpp"

#include "object/object.hpp"

#include <cstring>

using namespace std;

namespace oos {

statement_helper::statement_helper()
  : type_(UNKNOWN)
  , object_(0)
  , param_count_(0)
{}

statement_helper::~statement_helper()
{}

std::string statement_helper::create(object *o, const std::string &table_name, statement_helper::t_type type)
{
  type_ = type;
  object_ = o;
  param_count_ = 0;
  statement_.str("");
  switch (type_) {
    case CREATE:
      statement_ << "CREATE TABLE IF NOT EXISTS " << table_name << " (";
      break;
    case SELECT:
      statement_ << "SELECT ";
      break;
    case INSERT:
      statement_ << "INSERT INTO " << table_name << " (";
      break;
    case UPDATE:
      statement_ << "UPDATE " << table_name << " SET ";
      break;
    case DEL:
      statement_ << "DELETE FROM " << table_name << ";";
      return statement_.str();
    case DROP:
      statement_ << "DROP TABLE " << table_name << ";";
      return statement_.str();
    default:
      break;
  }
  o->write_to(this);
  
  string stmt = statement_.str();

  switch (type_) {
    case CREATE:
      // remove last char (,)
      stmt = stmt.substr(0, stmt.size() - 1);
      stmt += ");";
      break;
    case SELECT:
      // remove last char (,)
      stmt = stmt.substr(0, stmt.size() - 1);
      stmt += " FROM " + table_name;
      break;
    case INSERT:
      stmt = stmt.substr(0, stmt.size() - 1);
      stmt += ") VALUES (";
      while (--param_count_) {
        stmt += "?, ";
      }
      stmt += "?);";
      break;
    case UPDATE:
      stmt = stmt.substr(0, stmt.size() - 1);
      stmt += ";";
      break;
    case DEL:
      break;
    case DROP:
      break;
    default:
      break;
  }
  return stmt;
}

void statement_helper::write_char(const char *id, char)
{
  ++param_count_;
  switch (type_) {
    case CREATE:
      statement_ << id << " CHARACTER(1),";
      break;
    case SELECT:
      statement_ << id << ",";
      break;
    case INSERT:
      statement_ << id << ",";
      break;
    case UPDATE:
      statement_ << id << "=?,";
      break;
    case DEL:
      break;
    case DROP:
      break;
    default:
      break;
  }
}

void statement_helper::write_float(const char *id, float)
{
  ++param_count_;
  switch (type_) {
    case CREATE:
      statement_ << id << " REAL,";
      break;
    case SELECT:
      statement_ << id << ",";
      break;
    case INSERT:
      statement_ << id << ",";
      break;
    case UPDATE:
      statement_ << id << "=?,";
      break;
    case DEL:
      break;
    case DROP:
      break;
    default:
      break;
  }
}

void statement_helper::write_double(const char *id, double)
{
  ++param_count_;
  switch (type_) {
    case CREATE:
      statement_ << id << " REAL,";
      break;
    case SELECT:
      statement_ << id << ",";
      break;
    case INSERT:
      statement_ << id << ",";
      break;
    case UPDATE:
      statement_ << id << "=?,";
      break;
    case DEL:
      break;
    case DROP:
      break;
    default:
      break;
  }
}

void statement_helper::write_int(const char *id, int)
{
  ++param_count_;
  switch (type_) {
    case CREATE:
      statement_ << id << " INTEGER,";
      break;
    case SELECT:
      statement_ << id << ",";
      break;
    case INSERT:
      statement_ << id << ",";
      break;
    case UPDATE:
      statement_ << id << "=?,";
      break;
    case DEL:
      break;
    case DROP:
      break;
    default:
      break;
  }
}

void statement_helper::write_long(const char *id, long)
{
  ++param_count_;
  switch (type_) {
    case CREATE:
      statement_ << id << " INTEGER";
      if (strcmp(id, "id") == 0) {
        statement_ << " PRIMARY KEY";
      }
      statement_ << ",";
      break;
    case SELECT:
      statement_ << id << ",";
      break;
    case INSERT:
      statement_ << id << ",";
      break;
    case UPDATE:
      statement_ << id << "=?,";
      break;
    case DEL:
      break;
    case DROP:
      break;
    default:
      break;
  }
}

void statement_helper::write_unsigned(const char *id, unsigned)
{
  ++param_count_;
  switch (type_) {
    case CREATE:
      statement_ << id << " INTEGER,";
      break;
    case SELECT:
      statement_ << id << ",";
      break;
    case INSERT:
      statement_ << id << ",";
      break;
    case UPDATE:
      statement_ << id << "=?,";
      break;
    case DEL:
      break;
    case DROP:
      break;
    default:
      break;
  }
}

void statement_helper::write_bool(const char *id, bool)
{
  ++param_count_;
  switch (type_) {
    case CREATE:
      statement_ << id << " INTEGER,";
      break;
    case SELECT:
      statement_ << id << ",";
      break;
    case INSERT:
      statement_ << id << ",";
      break;
    case UPDATE:
      statement_ << id << "=?,";
      break;
    case DEL:
      break;
    case DROP:
      break;
    default:
      break;
  }
}

void statement_helper::write_charptr(const char *id, const char*)
{
  ++param_count_;
  switch (type_) {
    case CREATE:
      statement_ << id << " TEXT,";
      break;
    case SELECT:
      statement_ << id << ",";
      break;
    case INSERT:
      statement_ << id << ",";
      break;
    case UPDATE:
      statement_ << id << "=?,";
      break;
    case DEL:
      break;
    case DROP:
      break;
    default:
      break;
  }
}

void statement_helper::write_string(const char *id, const std::string&)
{
  ++param_count_;
  switch (type_) {
    case CREATE:
      statement_ << id << " TEXT,";
      break;
    case SELECT:
      statement_ << id << ",";
      break;
    case INSERT:
      statement_ << id << ",";
      break;
    case UPDATE:
      statement_ << id << "=?,";
      break;
    case DEL:
      break;
    case DROP:
      break;
    default:
      break;
  }
}

void statement_helper::write_varchar(const char *id, const varchar_base &x)
{
  ++param_count_;
  switch (type_) {
    case CREATE:
      statement_ << id << " VARCHAR(" << x.capacity() << "),";
      break;
    case SELECT:
      statement_ << id << ",";
      break;
    case INSERT:
      statement_ << id << ",";
      break;
    case UPDATE:
      statement_ << id << "=?,";
      break;
    case DEL:
      break;
    case DROP:
      break;
    default:
      break;
  }
}

void statement_helper::write_object(const char *id, const object_base_ptr&)
{
  ++param_count_;
  switch (type_) {
    case CREATE:
      statement_ << id << " INTEGER,";
      break;
    case SELECT:
      statement_ << id << ",";
      break;
    case INSERT:
      statement_ << id << ",";
      break;
    case UPDATE:
      statement_ << id << "=?,";
      break;
    case DEL:
      break;
    case DROP:
      break;
    default:
      break;
  }
}

}
