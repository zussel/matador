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
#include <list>

namespace oos {

class object;

class create_action;
class insert_action;
class update_action;
class delete_action;
class drop_action;

/// @cond OOS_DEV
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
  
  /**
   * @brief Called with a create_action
   * 
   * When a new prototype is inserted into
   * the object_store a new create_action
   * with the prototype object is created.
   * 
   * @param a The create_action to visit.
   */
  virtual void visit(create_action *a) = 0;
  
  /**
   * @brief Called with a insert_action
   * 
   * When a new object inserted into the
   * object_store a new insert_action with
   * newly created object is created.
   * 
   * @param a The insert_action to visit.
   */
  virtual void visit(insert_action *a) = 0;
  
  /**
   * @brief Called with a update_action
   * 
   * When an object updated in the
   * object_store a new update_action
   * with modified object is created.
   * 
   * @param a The update_action to visit.
   */
  virtual void visit(update_action *a) = 0;
  
  /**
   * @brief Called with a delete_action
   * 
   * When an object deleted from the
   * object_store a new delete_action
   * with deleted object is created.
   * 
   * @param a The delete_action to visit.
   */
  virtual void visit(delete_action *a) = 0;
  
  /**
   * @brief Called with a drop_action
   * 
   * When a prototype is removed from
   * the object_store a new drop_action
   * with the prototype object is created.
   * 
   * @param a The drop_action to visit.
   */
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
  virtual ~action() {}
  
  /**
   * Interface to accept the action.
   * 
   * @param av The action_visitor
   */
  virtual void accept(action_visitor *av) = 0;
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
  create_action() {}
  virtual ~create_action() {}
};

/**
 * @class insert_action
 * @brief Action when inserting an object.
 * 
 * This action is used when an objected
 * is inserted into the database.
 */
class OOS_API insert_action : public action
{
public:
  typedef std::list<object*> object_list_t;
  typedef object_list_t::iterator iterator;
  typedef object_list_t::const_iterator const_iterator;

public:
  /**
   * Creates an insert_action.
   * 
   * @param t The type of the expected objects
   */
  explicit insert_action(const std::string &t);

  virtual ~insert_action();
  
  virtual void accept(action_visitor *av);

  /**
   * Return the object type
   * of the action.
   *
   * @return The object type of the action
   */
  std::string type() const;

  iterator begin();
  const_iterator begin() const;
  
  iterator end();
  const_iterator end() const;

  bool empty() const;

  iterator find(long id);
  const_iterator find(long id) const;

  void push_back(object *o);

  iterator erase(iterator i);
private:
  std::string type_;
  object_list_t object_list_;
};


/**
 * @class update_action
 * @brief Action when updating an object.
 * 
 * This action is used when an objected
 * is updated on the database.
 */
class OOS_API update_action : public action
{
public:
  /**
   * Creates an update_action.
   * 
   * @param o The updated object.
   */
  update_action(object *o)
    : obj_(o)
  {}

  virtual ~update_action() {}
  
  virtual void accept(action_visitor *av)
  {
    av->visit(this);
  }

  /**
   * The object of the action.
   */
  object* obj();

  /**
   * The object of the action.
   */
  const object* obj() const;

private:
  object *obj_;
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
  /**
   * Creates an delete_action.
   * 
   * @param o The deleted object.
   */
  delete_action(const char *classname, long id);

  virtual ~delete_action();
  
  virtual void accept(action_visitor *av);

  /**
   * Return the class name of the
   * object.
   * 
   * @return The class name.
   */
  const char* classname() const;

  /**
   * The id of the object of the action.
   * 
   * @return The id of the deleted object.
   */
  long id() const;

private:
  std::string classname_;
  long id_;
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
  drop_action() {}
  virtual ~drop_action() {}
};

/// @endcond
}

#endif /* ACTION_HPP */
