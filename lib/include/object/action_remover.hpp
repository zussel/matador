//
// Created by sascha on 12.03.16.
//

#ifndef OOS_ACTION_REMOVER_HPP
#define OOS_ACTION_REMOVER_HPP

#include "object/action_visitor.hpp"
#include "object/action.hpp"

#include <vector>

namespace oos {

class object_proxy;

class action_remover : public action_visitor
{
public:
  typedef std::shared_ptr<action> action_ptr;
  typedef std::vector<action_ptr> t_action_vactor;

public:
  action_remover(t_action_vactor &actions);
  virtual ~action_remover() {}

  bool remove(t_action_vactor::size_type index, object_proxy *proxy);

  virtual void visit(insert_action *a);
  virtual void visit(update_action *a);
  virtual void visit(delete_action *a);

private:
  t_action_vactor &actions_;
  t_action_vactor::size_type index_;
  object_proxy *proxy_ = nullptr;
};

}

#endif //OOS_ACTION_REMOVER_HPP
