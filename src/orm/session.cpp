#include "matador/orm/session.hpp"

namespace matador {


session::session(persistence &p)
  : persistence_(p)
  , observer_(new session_transaction_observer(*this))
{

}

void session::flush()
{
  persistence_.conn().begin();
  for (auto const &i : persistence_.proxy_change_queue_) {
    auto it = persistence_.find_table(i.proxy->node()->type());
    if (it == persistence_.end()) {
      continue;
    }
    switch (i.action) {
      case persistence::proxy_change_action::INSERT:
        it->second->insert(i.proxy);
        break;
      case persistence::proxy_change_action::UPDATE:
        it->second->update(i.proxy);
        break;
      case persistence::proxy_change_action::REMOVE:
        it->second->remove(i.proxy);
        persistence_.proxies_to_delete_.erase(i.proxy);
        delete i.proxy;
        break;
    }
  }
  persistence_.conn().commit();
  persistence_.proxy_change_queue_.clear();
  persistence_.proxy_identifier_map_.clear();
}

void session::load()
{
  prototype_iterator first = persistence_.store().begin();
  prototype_iterator last = persistence_.store().end();
  while (first != last) {
    const prototype_node &node = (*first++);
    load(node);
  }
  persistence_.proxy_change_queue_.clear();
}

void session::load(const std::string &name)
{
  auto i = persistence_.store().find(name.c_str());
  if (i == persistence_.store().end()) {
    throw_object_exception("couldn't find prototype node");
  }

  load(*i);
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

void session::load(const prototype_node &node)
{
  if (node.is_abstract()) {
    return;
  }

  // find corresponding table and load entities
  auto i = persistence_.find_table(node.type());
  if (i == persistence_.end()) {
    // Todo: replace with persistence exception
    throw_object_exception("couldn't find table");
  }
  load(i->second);
}

session::session_transaction_observer::session_transaction_observer(session &s)
  : session_(s)
{}

void session::session_transaction_observer::on_begin()
{
}

void session::session_transaction_observer::on_commit(transaction::t_action_vector &actions)
{
  session_.persistence_.conn().begin();

  for (transaction::action_ptr &actptr : actions) {
    actptr->accept(this);
  }
  session_.persistence_.conn().commit();
}

void session::session_transaction_observer::on_rollback()
{
  session_.persistence_.conn().rollback();
}

void session::session_transaction_observer::visit(insert_action *act)
{
  auto i = session_.persistence_.find_table(act->type());
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

void session::session_transaction_observer::visit(update_action *act)
{
  auto i = session_.persistence_.find_table(act->proxy()->node()->type());
  if (i == session_.persistence_.end()) {
    // Todo: can't find table: give warning
    return;
  }

  i->second->update(act->proxy());
}

void session::session_transaction_observer::visit(delete_action *act)
{
  auto i = session_.persistence_.find_table(act->proxy()->node()->type());
  if (i == session_.persistence_.end()) {
    std::cout << "session: couldn't find table for type '" << act->proxy()->node()->type() << "'\n";
    // Todo: can't find table: give warning
    return;
  }

  i->second->remove(act->proxy());

  act->mark_deleted();
}


}