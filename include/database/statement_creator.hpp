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

#include "object/object.hpp"

#include <sstream>
#include <cstring>

namespace oos {

/// @cond OOS_DEV

class statement_creator
{
public:
  virtual ~statement_creator() {}

  virtual std::string create(object *o, const char *table_name, const char *where_clause) = 0;

protected:
  virtual void begin(const char *table_name) = 0;
  virtual bool step() { return false; }
  virtual void end(const char *table_name, const char *where_clause) = 0;

  std::stringstream& statement_stream() { return statement_; }

private:
  std::stringstream statement_;
};

template < class T >
class statement_field_creator : public statement_creator
{
public:
  statement_field_creator() : first_(true) {}
  virtual ~statement_field_creator() {}

protected:
  typedef T type_provider;

  virtual void write_field(const char *id, const char *type) = 0;
  virtual void write_pk_field(const char *id, const char *type) { write_field(id, type); }

  const type_provider& types() const { return types_; }

  virtual void begin(const char *) { first_ = true; }

  bool is_first() const { return first_; }
  void first(bool f) { first_ = f; }

public:
  template < class V >
  void write(const char *id, const V &x)
  {
    write_field(id, types_.type_string(x));
  }
	void write(const char *id, long x)
  {
    if (strcmp(id, "id") == 0) {
      write_pk_field(id, types_.type_string(x));
    } else {
      write_field(id, types_.type_string(x));
    }
  }

private:
  type_provider types_;
  bool first_;
};

template < class T >
class create_statement_creator : public statement_field_creator<T>
{
public:
  typedef statement_field_creator<T> base_creator;

public:
  create_statement_creator() {}
  virtual ~create_statement_creator() {}

  virtual std::string create(object *o, const char *table_name, const char*)
  {
    begin(table_name);

    o->serialize(*this);

    end(table_name, 0);

    return this->statement_stream().str();
  }

protected:
  virtual void begin(const char *table_name)
  {
    base_creator::begin(table_name);
    this->statement_stream() << T::create_postfix() << " " << table_name << " (";
  }

  virtual void end(const char *, const char *)
  {
    this->statement_stream() << ");";
  }

private:
  virtual void write_field(const char *id, const char *type)
  {
    if (!this->is_first()) {
      this->statement_stream() << ", ";
    } else {
      this->first(false);
    }
    this->statement_stream() << id << " " << type;
  }

  virtual void write_pk_field(const char *id, const char *type)
  {
    base_creator::write_pk_field(id, type);
    this->statement_stream() << " " << T::primary_key_prefix();
  }
};

template < class T >
class select_statement_creator : public statement_field_creator<T>
{
public:
  typedef statement_field_creator<T> base_creator;

public:
  select_statement_creator() {}
  virtual ~select_statement_creator() {}

  virtual std::string create(object *o, const char *table_name, const char *where_clause)
  {
    begin(table_name);

    o->serialize(*this);

    end(table_name, where_clause);

    return this->statement_stream().str();
  }

protected:
  virtual void begin(const char *table_name)
  {
    base_creator::begin(table_name);
    this->statement_stream() << T::select_postfix() << " ";
  }

  virtual void end(const char *table_name, const char *where_clause)
  {
    this->statement_stream() << " FROM " << table_name;
    if (where_clause) {
      this->statement_stream() << " WHERE " << where_clause;
    }
    this->statement_stream() << ";";
  }

private:
  virtual void write_field(const char *id, const char *)
  {
    if (!this->is_first()) {
      this->statement_stream() << ", ";
    } else {
      this->first(false);
    }
    this->statement_stream() << id;
  }
};

template < class T >
class insert_statement_creator : public statement_field_creator<T>
{
public:
  typedef statement_field_creator<T> base_creator;

public:
  insert_statement_creator()
    : second_(false)
  {}
  virtual ~insert_statement_creator() {}

  virtual std::string create(object *o, const char *table_name, const char*)
  {
    begin(table_name);

    do {
      o->serialize(*this);
    } while (step());

    end(table_name, 0);

    return this->statement_stream().str();
  }

private:
  virtual void write_field(const char *id, const char *)
  {
    if (!this->is_first()) {
      this->statement_stream() << ", ";
    } else {
      this->first(false);
    }
    if (!second_) {
      this->statement_stream() << id;
    } else {
      this->statement_stream() << "?";
    }
  }
  
  virtual void begin(const char *table_name)
  {
    base_creator::begin(table_name);
    second_ = false;
    this->statement_stream() << T::insert_postfix() << " " << table_name << " (";
  }

  virtual bool step()
  {
    if (!second_) {
      this->statement_stream() << ") VALUES (";
      second_ = true;
      this->first(true);
      return second_;
    } else {
      return !second_;
    }
    
  }

  virtual void end(const char *, const char *)
  {
    second_ = false;
    this->statement_stream() << ");";
  }

private:
  bool second_;
};

template < class T >
class update_statement_creator : public statement_field_creator<T>
{
public:
  typedef statement_field_creator<T> base_creator;

public:
  update_statement_creator() {}
  virtual ~update_statement_creator() {}

  virtual std::string create(object *o, const char *table_name, const char *where_clause)
  {
    begin(table_name);

    o->serialize(*this);

    end(table_name, where_clause);

    return this->statement_stream().str();
  }

private:
  virtual void write_field(const char *id, const char *)
  {
    if (!this->is_first()) {
      this->statement_stream() << ", ";
    } else {
      this->first(false);
    }
    this->statement_stream() << id << "=?";
  }

  virtual void begin(const char *table_name)
  {
    base_creator::begin(table_name);
    this->statement_stream() << T::update_postfix() << " " << table_name << " SET ";
  }
  
  virtual void end(const char *, const char *where_clause)
  {
    if (where_clause) {
      this->statement_stream() << " WHERE " << where_clause;
    }
    this->statement_stream() << ";";
  }
};

template < class T >
class delete_statement_creator : public statement_creator
{
public:
  delete_statement_creator() {}
  virtual ~delete_statement_creator() {}

  virtual std::string create(object *, const char *table_name, const char *where_clause)
  {
    begin(table_name);

    end(table_name, where_clause);
    return this->statement_stream().str();
  }
  
protected:
  virtual void begin(const char *table_name)
  {
    this->statement_stream() << T::delete_postfix() << " " << table_name << " ";
  }

  virtual void end(const char *, const char *where_clause)
  {
    if (where_clause) {
      this->statement_stream() << " WHERE " << where_clause;
    }
    this->statement_stream() << ";";
  }
};

template < class T >
class drop_statement_creator : public statement_creator
{
public:
  drop_statement_creator() {}
  virtual ~drop_statement_creator() {}

  virtual std::string create(object *, const char *table_name, const char*)
  {
    begin(table_name);
    
    end(table_name, 0);

    return this->statement_stream().str();
  }

protected:
  virtual void begin(const char *table_name)
  {
    this->statement_stream() << T::drop_postfix() << " " << table_name;
  }

  virtual void end(const char * /*table_name*/, const char * /*where_clause*/)
  {
    this->statement_stream() << ";";
  }
};

/// @endcond
}

#endif /* STATEMENT_CREATOR_HPP */
