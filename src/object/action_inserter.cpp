#include "matador/object/action_inserter.hpp"
#include "matador/object/object_proxy.hpp"
#include "matador/object/prototype_node.hpp"
#include "matador/object/insert_action.hpp"

namespace matador {

action_inserter::action_inserter(t_action_vactor &actions)
  : actions_(actions)
{ }

action_inserter::t_action_vactor::size_type action_inserter::insert(object_proxy *proxy) {
  proxy_ = proxy;
  inserted_ = false;
  t_action_vactor::size_type end = actions_.get().size();
  for (t_action_vactor::size_type i = 0; i < end; ++i) {
//  while (first != last) {
    actions_.get().at(i)->accept(this);

//    (*first)->accept(this);
    if (inserted_) {
      return i;
    }
  }
  if (!inserted_) {
    auto ia = std::make_shared<insert_action>(type(proxy));
    ia->push_back(proxy_);
    actions_.get().push_back(ia);
    return actions_.get().size() - 1;
  }
  return actions_.get().size();
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

void* action_inserter::object(object_proxy *proxy) const
{
  return proxy->obj();
}

const char* action_inserter::type(object_proxy *proxy) const
{
  return proxy->node()->type();
}

}