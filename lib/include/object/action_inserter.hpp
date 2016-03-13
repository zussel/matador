//
// Created by sascha on 11.03.16.
//

#ifndef OOS_ACTION_INSERTER_HPP
#define OOS_ACTION_INSERTER_HPP

#include "object/action_visitor.hpp"
#include "object/action.hpp"

#include <vector>

namespace oos {

class action_inserter : public action_visitor
{
public:
  typedef std::shared_ptr<action> action_ptr;
  typedef std::vector<action_ptr> t_action_vactor;
  typedef t_action_vactor::iterator action_iterator;

public:
  action_inserter(t_action_vactor &actions);

  virtual ~action_inserter() { }

  action_iterator insert(object_proxy *proxy);

  virtual void visit(insert_action *a);

  virtual void visit(update_action *a);

  virtual void visit(delete_action *a);

private:
  t_action_vactor &actions_;
  object_proxy *proxy_ = nullptr;
  bool inserted_ = false;
};

}

#endif //OOS_ACTION_INSERTER_HPP
