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

#ifdef _MSC_VER
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
#include <memory>

namespace oos {

class serializable;
class object_proxy;
class basic_identifier;
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
   * @internal
   * @brief Called with a create_action
   * 
   * When a new prototype is inserted into
   * the object_store a new create_action
   * with the prototype serializable is created.
   * 
   * @param a The create_action to visit.
   */
  virtual void visit(create_action *a) = 0;
  
  /**
   * @brief Called with a insert_action
   * 
   * When a new serializable inserted into the
   * object_store a new insert_action with
   * newly created serializable is created.
   * 
   * @param a The insert_action to visit.
   */
  virtual void visit(insert_action *a) = 0;
  
  /**
   * @brief Called with a update_action
   * 
   * When an serializable updated in the
   * object_store a new update_action
   * with modified serializable is created.
   * 
   * @param a The update_action to visit.
   */
  virtual void visit(update_action *a) = 0;
  
  /**
   * @brief Called with a delete_action
   * 
   * When an serializable deleted from the
   * object_store a new delete_action
   * with deleted serializable is created.
   * 
   * @param a The delete_action to visit.
   */
  virtual void visit(delete_action *a) = 0;
  
  /**
   * @brief Called with a drop_action
   * 
   * When a prototype is removed from
   * the object_store a new drop_action
   * with the prototype serializable is created.
   * 
   * @param a The drop_action to visit.
   */
  virtual void visit(drop_action *a) = 0;
};

/**
 * @internal
 * @class action
 * @brief Base class for all sql relevant actions
 *
 * The action class is the base class for all sql
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
 * @internal
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
 * @internal
 * @class insert_action
 * @brief Action when inserting an serializable.
 * 
 * This action is used when an objected
 * is inserted into the database.
 */
class OOS_API insert_action : public action
{
public:
  typedef std::list<object_proxy*> object_proxy_list_t;
  typedef object_proxy_list_t::iterator iterator;
  typedef object_proxy_list_t::const_iterator const_iterator;

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
   * Return the serializable type
   * of the action.
   *
   * @return The serializable type of the action
   */
  std::string type() const;

  iterator begin();
  const_iterator begin() const;
  
  iterator end();
  const_iterator end() const;

  bool empty() const;

  iterator find(unsigned long id);
  const_iterator find(unsigned long id) const;

  void push_back(object_proxy *proxy);

  iterator erase(iterator i);
private:
  std::string type_;
  object_proxy_list_t object_proxy_list_;
};


/**
 * @internal
 * @class update_action
 * @brief Action when updating an serializable.
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
   * @param o The updated serializable.
   */
  update_action(object_proxy *proxy)
    : proxy_(proxy)
  {}

  virtual ~update_action() {}
  
  virtual void accept(action_visitor *av)
  {
    av->visit(this);
  }

  /**
   * The serializable of the action.
   */
  object_proxy* proxy();

  /**
   * The serializable of the action.
   */
  const object_proxy* proxy() const;

private:
  object_proxy *proxy_;
};

/**
 * @internal
 * @class delete_action
 * @brief Action when deleting an serializable.
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
   * @param classname The serializable type name.
   * @param id The id of the deleted serializable.
   */
  delete_action(const char *classname, unsigned long id, basic_identifier *pk);

  virtual ~delete_action();
  
  virtual void accept(action_visitor *av);

  /**
   * Return the class name of the
   * serializable.
   * 
   * @return The class name.
   */
  const char* classname() const;

  /**
   * The primary key of the serializable of the action.
   * 
   * @return The primary key of the deleted serializable.
   */
  basic_identifier * pk() const;

  unsigned long id() const;

private:
  std::string classname_;
  unsigned long id_;
  std::unique_ptr<basic_identifier> pk_;
};

/**
 * @internal
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
