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

#ifndef TABLE_HPP
#define TABLE_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

#include "object/object_atomizer.hpp"

#include "database/statement.hpp"

namespace oos {

class database;
class statement_impl;
class object;
class object_container;
class object_base_ptr;
class object_store;
struct prototype_node;

/// @cond OOS_DEV

class OOS_API table
  : public generic_object_reader<table>
  , public generic_object_writer<table>
{
public:
  explicit table(database &db, const prototype_node &node);
  ~table();

  void load(object_store &ostore);
  void insert(object *obj_);
  void update(object *obj_);
  void remove(object *obj_);

  template < class T >
  void read_value(const char *, T &)
  {
  }
  void read_value(const char *, char *, int) {}

  template < class T >
  void write_value(const char *, const T &x)
  {
    if (inserting_) {
      insert_->bind(++column_, x);
    } else {
      update_->bind(++column_, x);
    }
  }
  void write_value(const char *, const char *x, int s)
  {
    if (inserting_) {
      insert_->bind(++column_, x, s);
    } else {
      update_->bind(++column_, x, s);
    }
  }
  void write_value(const char *, const varchar_base &) {}
  void write_value(const char *, const object_base_ptr &) {}
  void write_value(const char *, const object_container &) {}

private:
  statement_impl* select_;
  statement_impl* insert_;
  statement_impl* update_;
  statement_impl* delete_;

  const prototype_node &node_;
  int column_;
  object *object_;
  bool inserting_;
};

///@endcond

}

#endif /* TABLE_HPP */
