//
// Created by sascha on 11.03.16.
//

#include "object/action_inserter.hpp"
#include "object/object_proxy.hpp"
#include "object/prototype_node.hpp"

namespace oos {

action_inserter::action_inserter(t_action_vactor &actions)
  : actions_(actions)
{ }

action_inserter::action_iterator action_inserter::insert(object_proxy *proxy) {
  proxy_ = proxy;
  inserted_ = false;
  action_iterator first = actions_.begin();
  action_iterator last = actions_.end();
  while (first != last) {
    (*first)->accept(this);
    if (inserted_) {
      return first;
    } else {
      ++first;
    }
  }
  if (!inserted_) {
    auto ia = std::make_shared<insert_action>(proxy_->node()->type());
    ia->push_back(proxy_);
    return actions_.insert(actions_.end(), ia);
  }
  return last;
}

void action_inserter::visit(insert_action *a) {
  // check (object) type of insert action
  // if type is equal to objects type
  // add serializable to action
  if (a->type() == proxy_->node()->type()) {
    a->push_back(proxy_);
    inserted_ = true;
  }
}

void action_inserter::visit(update_action *) {
  // error: object can't be updated before
  // it is inserted, throw error
}

void action_inserter::visit(delete_action *) {
  // error: object can't be deleted before
  // it is inserted, throw error
}

}