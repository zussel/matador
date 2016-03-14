//
// Created by sascha on 11.03.16.
//

#ifndef OOS_ACTION_INSERTER_HPP
#define OOS_ACTION_INSERTER_HPP

#include "object/action_visitor.hpp"
#include "object/action.hpp"
#include "object/object_proxy.hpp"
#include "object/prototype_node.hpp"
#include "object/insert_action.hpp"

#include <vector>

namespace oos {

class object_proxy;

class action_inserter : public action_visitor
{
public:
  typedef std::shared_ptr<action> action_ptr;
  typedef std::vector<action_ptr> t_action_vactor;
  typedef t_action_vactor::iterator action_iterator;

public:
  action_inserter(t_action_vactor &actions);

  virtual ~action_inserter() { }

  template < class T >
  action_iterator insert(object_proxy *proxy);

  virtual void visit(insert_action *a);
  virtual void visit(update_action *a);
  virtual void visit(delete_action *a);

private:
  t_action_vactor &actions_;
  object_proxy *proxy_ = nullptr;
  bool inserted_ = false;
};

template < class T >
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
    std::shared_ptr<insert_action> ia(new insert_action(proxy_->node()->type(), (T*)proxy->obj()));
    ia->push_back(proxy_);
    return actions_.insert(actions_.end(), ia);
  }
  return last;
}

}

#endif //OOS_ACTION_INSERTER_HPP
