//
// Created by sascha on 21.03.16.
//

#include "matador/orm/session.hpp"

namespace matador {


session::session(persistence &p)
  : persistence_(p)
  , observer_(new session_observer(*this))
{

}

void session::load()
{
  prototype_iterator first = persistence_.store().begin();
  prototype_iterator last = persistence_.store().end();
  while (first != last) {
    const prototype_node &node = (*first++);
    if (node.is_abstract()) {
      continue;
    }

    // find corresponding table and load entities
    persistence::t_table_map::iterator i = persistence_.find_table(node.type());
    if (i == persistence_.end()) {
      // Todo: replace with persistence exception
      throw object_exception("couldn't find table");
    }
//    std::cout << "loading table " << i->second->name() << "\n";
    load(i->second);
  }
}

transaction session::begin()
{
  transaction tr(persistence_.store(), observer_);
  tr.begin();
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

void session::load(const persistence::table_ptr &table)
{
  table->load(persistence_.store());
}

session::session_observer::session_observer(session &s)
  : session_(s)
{}

void session::session_observer::on_begin()
{
}

void session::session_observer::on_commit(transaction::t_action_vector &actions)
{
  session_.persistence_.conn().begin();

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
    std::cout << "session: couldn't find table for type '" << act->proxy()->node()->type() << "'\n";
    // Todo: can't find table: give warning
    return;
  }

  i->second->remove(act->proxy());

  act->mark_deleted();
}


}