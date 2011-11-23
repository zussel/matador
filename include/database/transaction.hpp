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

#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

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

#include "tools/byte_buffer.hpp"
#include "object/object_store.hpp"
#include "object/object_serializer.hpp"
#include "database/action.hpp"

#include <memory>
#include <list>
#include <set>

namespace oos {

class database;
class transaction;

class OOS_API transaction_impl : public object_observer
{
public:
  transaction_impl(transaction &tr);
  virtual ~transaction_impl();

  virtual void on_insert(object *o);
  virtual void on_update(object *o);
  virtual void on_delete(object *o);
  
  virtual void rollback();
  virtual void commit();

private:
  transaction &tr_;
};

class OOS_API transaction
{
public:
  transaction(database *db);
  ~transaction();
  
  long id() const;

  void start();
  void commit();
  void rollback();

  database* db();
  const database* db() const;

private:
  class backup_visitor : public action_visitor
  {
  public:
    backup_visitor()
      : buffer_(NULL)
    {}
    virtual ~backup_visitor() {}

    bool backup(action *act, byte_buffer *buffer);

    virtual void visit(create_action *) {}
    virtual void visit(insert_action *a);
    virtual void visit(update_action *a);
    virtual void visit(delete_action *a);  
    virtual void visit(drop_action *) {}

  private:
    byte_buffer *buffer_;
    object_serializer serializer_;
  };

  class restore_visitor : public action_visitor
  {
  public:
    restore_visitor()
      : buffer_(NULL)
      , ostore_(NULL)
    {}
    virtual ~restore_visitor() {}

    bool restore(action *act, byte_buffer *buffer, object_store *ostore);

    virtual void visit(create_action *) {}
    virtual void visit(insert_action *a);
    virtual void visit(update_action *a);
    virtual void visit(delete_action *a);
    virtual void visit(drop_action *) {}

  private:
    byte_buffer *buffer_;
    object_store *ostore_;
    object_serializer serializer_;
  };

private:
  typedef std::set<long> id_set_t;

  typedef std::list<action*> action_list_t;
  typedef action_list_t::iterator iterator;
  typedef action_list_t::const_iterator const_iterator;

  friend class transaction_impl;
  friend class object_store;
  friend class database;
  
  void backup(action *a);
  void restore(action *a);

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;

  iterator erase(iterator i);

  bool empty() const;
  size_t size() const;

private:
  static long id_counter;

private:
  database *db_;
  long id_;
  
  transaction_impl *impl_;
  
  id_set_t id_set_;
  action_list_t action_list_;

  byte_buffer object_buffer_;
  backup_visitor backup_visitor_;
  restore_visitor restore_visitor_;
};

}

#endif /* TRANSACTION_HPP */
