//
// Created by sascha on 12.03.16.
//

#ifndef OOS_ACTION_REMOVER_HPP
#define OOS_ACTION_REMOVER_HPP

#ifdef _MSC_VER
#ifdef oos_object_EXPORTS
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

#include "oos/object/action_visitor.hpp"
#include "oos/object/action.hpp"

#include <vector>

namespace oos {

/// @cond OOS_DEV

class object_proxy;

class OOS_API action_remover : public action_visitor
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

/// @endcond

}

#endif //OOS_ACTION_REMOVER_HPP
