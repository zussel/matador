#ifndef OOS_ACTION_REMOVER_HPP
#define OOS_ACTION_REMOVER_HPP

#include "matador/object/export.hpp"

#include "matador/object/action_visitor.hpp"
#include "matador/object/action.hpp"

#include <vector>

namespace matador {

/// @cond MATADOR_DEV

class object_proxy;

class MATADOR_OBJECT_API action_remover : public action_visitor
{
public:
  typedef std::shared_ptr<action> action_ptr;
  typedef std::vector<action_ptr> t_action_vector;

public:
  explicit action_remover(t_action_vector &actions);

  bool remove(t_action_vector::size_type index, object_proxy *proxy);

  void visit(insert_action *a) override;
  void visit(update_action *a) override;
  void visit(delete_action *a) override;

private:
  t_action_vector &actions_;
  t_action_vector::size_type index_;
  object_proxy *proxy_ = nullptr;
};

/// @endcond

}

#endif //OOS_ACTION_REMOVER_HPP
