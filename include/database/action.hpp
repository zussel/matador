#ifndef ACTION_HPP
#define ACTION_HPP

#include "object/object.hpp"

namespace oos {

//class object;

class insert_action;
class update_action;
class delete_action;

class action_visitor
{
public:
  virtual ~action_visitor() {}
  
  virtual void visit(insert_action *a) = 0;
  virtual void visit(update_action *a) = 0;
  virtual void visit(delete_action *a) = 0;
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
    : action(NULL)
    , id_(o ? o->id() : 0)
  {}
  virtual ~delete_action() {}
  
  virtual void accept(action_visitor *av)
  {
    av->visit(this);
  }

  long id() const { return id_; }

private:
  long id_;
};

}

#endif /* ACTION_HPP */
