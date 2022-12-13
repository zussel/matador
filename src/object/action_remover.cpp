#include "matador/utils/identifier_resolver.hpp"

#include "matador/object/action_remover.hpp"
#include "matador/object/insert_action.hpp"
#include "matador/object/update_action.hpp"
#include "matador/object/object_proxy.hpp"

namespace matador {

action_remover::action_remover(t_action_vector &actions)
  : actions_(actions)
{}


bool action_remover::remove(t_action_vector::size_type index, object_proxy *proxy)
{
  proxy_ = proxy;
  index_ = index;
  action_ptr aptr(actions_.at(index));
  aptr->accept(this);
  proxy_ = 0;
  index_ = 0;
  return true;
}

void action_remover::visit(insert_action *a)
{
  /***********
   *
   * an insert action was found
   * try to find serializable with given
   * id and delete and remove it
   * from insert action
   *
   ***********/
  auto i = a->find(proxy_->id());
  if (i != a->end()) {
    a->erase(i);
  }
  if (a->empty()) {
    actions_.erase(actions_.begin() + index_);
  }
}

void action_remover::visit(update_action *a)
{
  /***********
   *
   * an update action was found
   * replace this update action
   * with a new delete action
   * with this given serializable.
   *
   ***********/
//  if (a->proxy()->id() == id_) {
  if (a->proxy()->id() == proxy_->id()) {
    actions_.at(index_).reset(a->release_delete_action());
  }
}

void action_remover::visit(delete_action *) {}

}