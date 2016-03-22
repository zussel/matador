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

}

void session::session_observer::on_commit()
{

}

void session::session_observer::on_begin()
{

}

void session::session_observer::visit(insert_action *act)
{

}

void session::session_observer::visit(update_action *act)
{

}

void session::session_observer::visit(delete_action *act)
{

}


}