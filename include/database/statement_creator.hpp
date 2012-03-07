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

class statement_creator
{
public:
  virtual ~statement_creator() {}

  virtual statement_impl* create(database_impl *db, object *o, const std::string &table_name, const std::string &where_clause) = 0;

protected:
  virtual void begin(const char *cmd)
  {
    statement_.str("");
    statement_ << cmd << " ";
  }

  std::stringstream& statement_stream() { return statement_; }

private:
  std::stringstream statement_;
};

template < class T >
class statement_field_creator : public statement_creator, public object_atomizer
{
public:
  virtual ~statement_field_creator() {}

protected:
  typedef T type_provider;

  virtual void write_field(const char *id, const char *type) = 0;
  virtual void write_pk_field(const char *id, const char *type) { write_field(id, type); }

  const type_provider& types() const { return types_; }

private:
  virtual void write_char(const char *id, char x) { write_field(id, types_.type_string(x)); }
	virtual void write_float(const char *id, float x) { write_field(id, types_.type_string(x)); }
	virtual void write_double(const char *id, double x) { write_field(id, types_.type_string(x)); }
	virtual void write_int(const char *id, int x) { write_field(id, types_.type_string(x)); }
	virtual void write_long(const char *id, long x)
  {
    if (strcmp(id, "id") == 0) {
      write_pk_field(id, types_.type_string(x));
    } else {
      write_field(id, types_.type_string(x));
    }
  }
	virtual void write_unsigned(const char *id, unsigned x) { write_field(id, types_.type_string(x)); }
	virtual void write_bool(const char *id, bool x) { write_field(id, types_.type_string(x)); }
	virtual void write_charptr(const char *id, const char *x) { write_field(id, types_.type_string(x)); }
	virtual void write_string(const char *id, const std::string &x) { write_field(id, types_.type_string(x)); }
	virtual void write_varchar(const char *id, const varchar_base &x) { write_field(id, types_.type_string(x)); }
	virtual void write_object(const char *id, const object_base_ptr &x) { write_field(id, types_.type_string(x)); }

private:
  type_provider types_;
};

template < class T >
class create_statement_creator : public statement_field_creator<T>
{
public:
  typedef statement_field_creator<T> base_creator;

public:
  create_statement_creator() {}
  virtual ~create_statement_creator() {}

  virtual statement_impl* create(database_impl *db, object *o, const std::string &table_name, const std::string &where_clause)
  {
    begin(T::create_postfix(table_name));

    o->write_to(this);

    this->statement_stream() << ");";

    statement_impl *stmt = db->create_statement();
    stmt->prepare(this->statement_stream().str());
    return stmt;
  }

private:
  virtual void write_field(const char *id, const char *type)
  {
    if (!first_) {
      this->statement_stream() << ", ";
    } else {
      first_ = false;
    }
    this->statement_stream() << id << " " << type;
  }
  virtual void write_pk_field(const char *id, const char *type)
  {
    write_pk_field(id, type);
    this->statement_stream() << " " << T::primary_key_prefix();
  }

private:
  bool first_;
};

template < class T >
class select_statement_creator : public statement_field_creator<T>
{
public:
  typedef statement_field_creator<T> base_creator;

public:
  select_statement_creator() {}
  virtual ~select_statement_creator() {}

  virtual statement_impl* create(database_impl *db, object *o, const std::string &table_name, const std::string &where_clause)
  {
    begin(T::select_postfix());

    o->write_to(this);

    this->statement_stream() << " FROM " << table_name;
    if (!where_clause.empty()) {
      this->statement_stream() << " WHERE " << where_clause;
    }
    this->statement_stream() << ";";
    statement_impl *stmt = db->create_statement();
    stmt->prepare(this->statement_stream().str());
    return stmt;
  }

private:
  virtual void write_field(const char *id, const char *type)
  {
    if (!first_) {
      this->statement_stream() << ", ";
    } else {
      first_ = false;
    }
    this->statement_stream() << id;
  }
  
  virtual void begin(const char *cmd)
  {
    first_ = true;
    base_creator::begin(cmd);
  }
private:
  bool first_;
};

template < class T >
class insert_statement_creator : public statement_creator
{
public:
  typedef statement_field_creator<T> base_creator;

public:
  insert_statement_creator() {}
  virtual ~insert_statement_creator() {}

  virtual statement_impl* create(database_impl *db, object *o, const std::string &table_name, const std::string &where_clause)
  {
    begin(T::insert_postfix(table_name));

    o->write_to(this);

    this->statement_stream() << ") VALUES (" << value_stream_.str() << ");";
    statement_impl *stmt = db->create_statement();
    stmt->prepare(this->statement_stream().str());
    return stmt;
  }

private:
  virtual void write_field(const char *id, const char *type)
  {
    if (!first_) {
      this->statement_stream() << ", ";
      value_stream_ << ", ";
    } else {
      first_ = false;
    }
    this->statement_stream() << id;
    value_stream_ << "?";
  }
  
  virtual void begin(const char *cmd)
  {
    value_stream_.str("");
    base_creator::begin(cmd);
  }

private:
  bool first_;
  std::stringstream value_stream_;
};

template < class T >
class update_statement_creator : public statement_creator
{
public:
  typedef statement_field_creator<T> base_creator;

public:
  update_statement_creator() {}
  virtual ~update_statement_creator() {}

  virtual statement_impl* create(database_impl *db, object *o, const std::string &table_name, const std::string &where_clause)
  {
    begin(T::update_postfix(table_name));

    o->write_to(this);

    if (!where_clause.empty()) {
      this->statement_stream() << " WHERE " << where_clause;
    }
    this->statement_stream() << ";";
    statement_impl *stmt = db->create_statement();
    stmt->prepare(this->statement_stream().str());
    return stmt;
  }

private:
  virtual void write_field(const char *id, const char *type)
  {
    if (!first_) {
      this->statement_stream() << ", ";
    } else {
      first_ = false;
    }
    this->statement_stream() << id << "=?";
  }

  virtual void begin(const char *cmd)
  {
    base_creator::begin(cmd);
  }

private:
  bool first_;
};

template < class T >
class delete_statement_creator : public statement_creator
{
public:
  delete_statement_creator() {}
  virtual ~delete_statement_creator() {}

  virtual statement_impl* create(database_impl *db, object *o, const std::string &table_name, const std::string &where_clause)
  {
    begin(T::delete_postfix(table_name));

    if (!where_clause.empty()) {
      this->statement_stream() << " WHERE " << where_clause;
    }

    this->statement_stream() << ";";
    statement_impl *stmt = db->create_statement();
    stmt->prepare(this->statement_stream().str());
    return stmt;
  }
};

template < class T >
class drop_statement_creator : public statement_creator
{
public:
  drop_statement_creator() {}
  virtual ~drop_statement_creator() {}

  virtual statement_impl* create(database_impl *db, object *o, const std::string &table_name, const std::string &where_clause)
  {
    begin(T::drop_postfix(table_name));

    statement_impl *stmt = db->create_statement();
    stmt->prepare(this->statement_stream().str());
    return stmt;
  }
};

}

#endif /* STATEMENT_CREATOR_HPP */
