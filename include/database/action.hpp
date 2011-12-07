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

#ifndef ACTION_HPP
#define ACTION_HPP

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

#include <string>

namespace oos {

class object;

class create_action;
class insert_action;
class update_action;
class delete_action;
class drop_action;

/**
 * @class action_visitor
 * @brief The action_visitor class accepts all action types.
 *
 * The action_visitor class accepts all concrete action
 * type classes. It implements the visitor pattern. To have
 * a concrete visitor, a class must be derived from this
 * class and implement all virtual method.
 */
class OOS_API action_visitor
{
public:
  virtual ~action_visitor() {}
  
  virtual void visit(create_action *a) = 0;
  virtual void visit(insert_action *a) = 0;
  virtual void visit(update_action *a) = 0;
  virtual void visit(delete_action *a) = 0;
  virtual void visit(drop_action *a) = 0;
};

/**
 * @class action
 * @brief Base class for all database relevant actions
 *
 * The action class is the base class for all database
 * relevant actions like:
 *  - create (table)
 *  - insert
 *  - update
 *  - delete
 *  - drop (table)
 *
 * All these actions are accepted via the action_visitor
 * class, which implements the visitor pattern.
 */
class OOS_API action
{
public:
  /**
   * @brief Action type enumeration
   *
   * The enumeration consists of all
   * available action types.
   */
  typedef enum {
    CREATE = 0, /**<Enum type for create action. */
    INSERT,     /**<Enum type for insert action. */
    UPDATE,     /**<Enum type for update action. */
    DELETE,     /**<Enum type for delete action. */
    DROP        /**<Enum type for drop action. */
  } action_type;

  action(action_type t, object *o);
  virtual ~action();
  
  virtual void accept(action_visitor *av) = 0;

  action_type type() const;

  object* obj();
  const object* obj() const;

  long id() const;

private:
  action_type type_;
  object *obj_;
  long id_;
};

/**
 * @class create_action
 * @brief Action when creating a table.
 * 
 * This action is used when a table is
 * created.
 */
class create_action : public action
{
public:
  create_action()
    : action(action::CREATE, NULL)
  {}
  virtual ~create_action() {}
};

/**
 * @class insert_action
 * @brief Action when inserting an object.
 * 
 * This action is used when an objected
 * is inserted into the database.
 */
class insert_action : public action
{
public:
  insert_action(object *o)
    : action(action::INSERT, o)
  {}
  virtual ~insert_action() {}
  
  virtual void accept(action_visitor *av)
  {
    av->visit(this);
  }
};


/**
 * @class update_action
 * @brief Action when updating an object.
 * 
 * This action is used when an objected
 * is updated on the database.
 */
class update_action : public action
{
public:
  update_action(object *o)
    : action(action::UPDATE, o)
  {}
  virtual ~update_action() {}
  
  virtual void accept(action_visitor *av)
  {
    av->visit(this);
  }
};

/**
 * @class delete_action
 * @brief Action when deleting an object.
 * 
 * This action is used when an objected
 * is deleted from the database.
 */
class OOS_API delete_action : public action
{
public:
  delete_action(object *o);
  virtual ~delete_action();
  
  virtual void accept(action_visitor *av);

  std::string type() const;

private:
  std::string type_;
};

/**
 * @class drop_action
 * @brief Action when dropping a table.
 * 
 * This action is used when a table is
 * dropped.
 */
class drop_action : public action
{
public:
  drop_action()
    : action(action::DROP, NULL)
  {}
  virtual ~drop_action() {}
};

}

#endif /* ACTION_HPP */
