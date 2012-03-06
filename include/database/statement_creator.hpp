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

#ifndef STATEMENT_CREATOR_HPP
#define STATEMENT_CREATOR_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
#else
  #define OOS_API
#endif

//#include "database/database_impl.hpp"
#include "database/statement.hpp"

#include "object/object.hpp"

#include <sstream>

namespace oos {

class statement_creator : public object_atomizer
{
public:
  virtual ~statement_creator() {}

  virtual statement_impl* create(database_impl *db, object *o, const std::string &table_name, const std::string &where_clause) = 0;
};

template < class T >
class select_statement_creator : public statement_creator
{
public:
  select_statement_creator() {}
  virtual ~select_statement_creator() {}

  virtual statement_impl* create(database_impl *db, object *o, const std::string &table_name, const std::string &where_clause)
  {
    reset();
    o->write_to(this);

    statement_ << " FROM " << table_name;
    if (!where_clause.empty()) {
      statement_ << " WHERE " << where_clause;
    }
    statement_ << ";";
    statement_impl *stmt = db->create_statement();
    stmt->prepare(statement_.str());
    return stmt;
  }

private:
  virtual void write_char(const char *id, char x) { write(id, T::char_type_string()); }
	virtual void write_float(const char *id, float x) { write(id, T::float_type_string()); }
	virtual void write_double(const char *id, double x) { write(id, T::double_type_string()); }
	virtual void write_int(const char *id, int x) { write(id, T::int_type_string()); }
	virtual void write_long(const char *id, long x)
  {
    write(id, T::long_type_string());
    if (strcmp(id, "id") == 0) {
      statement_ << " " << T::primary_key_prefix();
    }
  }
	virtual void write_unsigned(const char *id, unsigned x) { write(id, T::unsigned_int_type_string()); }
	virtual void write_bool(const char *id, bool x) { write(id, T::bool_type_string()); }
	virtual void write_charptr(const char *id, const char *x) { write(id, T::char_ptr_type_string()); }
	virtual void write_string(const char *id, const std::string &x) { write(id, T::string_type_string()); }
	virtual void write_varchar(const char *id, const varchar_base &x) { write(id, T::varchar_type_string()); }
	virtual void write_object(const char *id, const object_base_ptr &x) { write(id, T::object_type_string()); }

private:
  void write(const char *id, const char *type)
  {
    if (!first_) {
      statement_ << ", ";
    } else {
      first_ = false;
    }
    statement_ << id << type;
  }

  void reset()
  {
    first_ = true;
    statement_.str("");
    statement_ << T::select_postfix() << " ";
  }
private:
  bool first_;
  object *object_;
  std::stringstream statement_;
};

template < class T >
class insert_statement_creator : public statement_creator
{
public:
  insert_statement_creator() {}
  virtual ~insert_statement_creator() {}

  virtual statement_impl* create(database_impl *db, object *o, const std::string &table_name, const std::string &where_clause)
  {
    return 0;
  }
};

}

#endif /* STATEMENT_CREATOR_HPP */