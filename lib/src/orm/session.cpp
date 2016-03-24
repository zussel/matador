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

void session::session_observer::on_rollback()
{
  // Todo: rollback transaction

}

void session::session_observer::on_commit(transaction::t_action_vactor &actions)
{
  for (transaction::action_ptr &actptr : actions) {
    actptr->accept(this);
  }
  // Todo: commit transaction

}

void session::session_observer::on_begin()
{
  // Todo: begin transaction
  connection &conn = session_.persistence_.conn();
  basic_dialect *dialect = conn.dialect();
  // Todo: conn.execute(dialect->begin());
  conn.execute(dialect->token(basic_dialect::BEGIN));
  session_.persistence_.conn();
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