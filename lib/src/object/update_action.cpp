//
// Created by sascha on 12.03.16.
//

#include "object/update_action.hpp"
#include "object/action_visitor.hpp"

namespace oos {

void update_action::accept(action_visitor *av)
{
  av->visit(this);
}

object_proxy* update_action::proxy()
{
  return proxy_;
}

const object_proxy* update_action::proxy() const
{
  return proxy_;
}

delete_action *update_action::release_delete_action()
{
  return delete_action_.release();
}

}