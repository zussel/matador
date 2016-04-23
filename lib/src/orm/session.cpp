//
// Created by sascha on 21.03.16.
//

#include "orm/session.hpp"

namespace oos {


session::session(persistence &p)
  : persistence_(p)
  , observer_(new session_observer(*this))
{

}

transaction session::begin()
{
  transaction tr(persistence_.store(), observer_);
  tr.begin();
//  return transactions_.top();

  return persistence_.store().current_transaction();
}

object_store &session::store()
{
  return persistence_.store();
}

const object_store &session::store() const
{
  return persistence_.store();
}

session::session_observer::session_observer(session &s)
  : session_(s)
{}

void session::session_observer::on_begin()
{
  session_.persistence_.conn().begin();
}

void session::session_observer::on_commit(transaction::t_action_vector &actions)
{
  for (transaction::action_ptr &actptr : actions) {
    actptr->accept(this);
  }
  session_.persistence_.conn().commit();
}

void session::session_observer::on_rollback()
{
  session_.persistence_.conn().rollback();
}

void session::session_observer::visit(insert_action *act)
{
  persistence::t_table_map::iterator i = session_.persistence_.find_table(act->type());
  if (i == session_.persistence_.end()) {
    // Todo: can't find table: give warning
    return;
  }

  insert_action::const_iterator first = act->begin();
  insert_action::const_iterator last = act->end();
  while (first != last) {
    i->second->insert((*first++));
  }

}

void session::session_observer::visit(update_action *act)
{
  persistence::t_table_map::iterator i = session_.persistence_.find_table(act->proxy()->node()->type());
  if (i == session_.persistence_.end()) {
    // Todo: can't find table: give warning
    return;
  }

  i->second->update(act->proxy());
}

void session::session_observer::visit(delete_action *act)
{
  persistence::t_table_map::iterator i = session_.persistence_.find_table(act->proxy()->node()->type());
  if (i == session_.persistence_.end()) {
    // Todo: can't find table: give warning
    return;
  }

  i->second->remove(act->proxy());
}


}