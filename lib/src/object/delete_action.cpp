//
// Created by sascha on 12.03.16.
//

#include "object/delete_action.hpp"
#include "object/action_visitor.hpp"

namespace oos {

delete_action::delete_action(const char *classname, unsigned long id, basic_identifier *pk)
  : classname_(classname)
  , id_(id)
  , pk_(pk)
{}

delete_action::~delete_action()
{}

void delete_action::accept(action_visitor *av)
{
  av->visit(this);
}

const char* delete_action::classname() const
{
  return classname_.c_str();
}

basic_identifier * delete_action::pk() const
{
  return pk_.get();
}

unsigned long delete_action::id() const
{
  return id_;
}

}