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

#include "object/object.hpp"

namespace oos {

//class object;

class create_action;
class insert_action;
class update_action;
class delete_action;
class drop_action;

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

class action
{
public:
  typedef enum {
    CREATE = 0,
    INSERT,
    UPDATE,
    DELETE,
    DROP
  } action_type;

  action(action_type t, object *o)
    : type_(t), obj_(o), id_((o ? o->id() : 0))
  {}
  virtual ~action() {}
  
  virtual void accept(action_visitor *av) = 0;

  action_type type() const { return type_; }

  object* obj() { return obj_; }
  const object* obj() const { return obj_; }

  long id() const { return id_; }

private:
  action_type type_;
  object *obj_;
  long id_;
};

class create_action : public action
{
public:
  create_action()
    : action(action::CREATE, NULL)
  {}
  virtual ~create_action() {}
};

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

class delete_action : public action
{
public:
  delete_action(object *o)
    : action(action::DELETE, o)
    , type_(o ? o->object_type() : "")
  {}
  virtual ~delete_action() {}
  
  virtual void accept(action_visitor *av)
  {
    av->visit(this);
  }

  std::string type() const { return type_; }

private:
  std::string type_;
};

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
