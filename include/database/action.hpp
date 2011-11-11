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
  action(object *o)
    : obj_(o)
  {}
  virtual ~action() {}
  
  virtual void accept(action_visitor *av) = 0;

  object* obj() { return obj_; }
  const object* obj() const { return obj_; }

private:
  object *obj_;
};

class create_action : public action
{
public:
  create_action()
    : action(NULL)
  {}
  virtual ~create_action() {}
};

class insert_action : public action
{
public:
  insert_action(object *o)
    : action(o)
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
    : action(o)
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
    : action(o)
    , id_(o ? o->id() : 0)
    , type_(o ? o->object_type() : "")
  {}
  virtual ~delete_action() {}
  
  virtual void accept(action_visitor *av)
  {
    av->visit(this);
  }

  long id() const { return id_; }
  std::string type() const { return type_; }

private:
  long id_;
  std::string type_;
};

class drop_action : public action
{
public:
  drop_action()
    : action(NULL)
  {}
  virtual ~drop_action() {}
};

}

#endif /* ACTION_HPP */
