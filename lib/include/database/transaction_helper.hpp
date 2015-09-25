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

#ifndef TRANSACTION_HELPER_HPP
#define TRANSACTION_HELPER_HPP

#include "object/object_serializer.hpp"

#include "database/action.hpp"
#include "database/transaction.hpp"

#include <unordered_map>

#include <memory>
#include <list>
#include <set>
#include <map>

namespace oos {

/// @cond OOS_DEV

class byte_buffer;

class backup_visitor : public action_visitor
{
public:
  backup_visitor()
    : buffer_(0)
    , object_(0)
  {}
  virtual ~backup_visitor() {}

  bool backup(action *act, const serializable *o, byte_buffer *buffer);

  virtual void visit(create_action *) {}
  virtual void visit(insert_action *a);
  virtual void visit(update_action *a);
  virtual void visit(delete_action *a);  
  virtual void visit(drop_action *) {}

private:
  byte_buffer *buffer_;
  const serializable *object_;
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

class action_inserter : public action_visitor
{
public:
  action_inserter(transaction::action_list_t &action_list)
    : action_list_(action_list)
    , proxy_(0)
    , inserted_(false)
  {}
  virtual ~action_inserter() {}

  transaction::iterator insert(object_proxy *proxy);

  virtual void visit(create_action*) {}
  virtual void visit(insert_action *a);
  virtual void visit(update_action *a);
  virtual void visit(delete_action *a);
  virtual void visit(drop_action*) {}

private:
  transaction::action_list_t &action_list_;
  object_proxy *proxy_;
  bool inserted_;
};

class action_remover : public action_visitor
{
public:
  action_remover(transaction::action_list_t &action_list)
    : action_list_(action_list)
    , id_(0)
  {}
  virtual ~action_remover() {}

  bool remove(transaction::iterator i, object_proxy *proxy);

  virtual void visit(create_action*) {}
  virtual void visit(insert_action *a);
  virtual void visit(update_action *a);
  virtual void visit(delete_action *a);
  virtual void visit(drop_action*) {}

private:
  transaction::action_list_t &action_list_;
  transaction::iterator iter_;
  object_proxy *proxy_;
  unsigned long id_;
};
/// @endcond

}

#endif /* TRANSACTION_HELPER_HPP */
