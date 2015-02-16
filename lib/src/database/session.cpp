/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "database/session.hpp"
#include "database/result.hpp"
#include "database/database_factory.hpp"
#include "database/database_sequencer.hpp"
#include "database/action.hpp"
#include "database/transaction.hpp"
#include "database/memory_database.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

#include <stdexcept>

using namespace std;

namespace oos {

session::session(object_store &ostore, const std::string &dbstring)
  : ostore_(ostore)
{
  // parse dbstring
  std::string::size_type pos = dbstring.find(':');
  type_ = dbstring.substr(0, pos);
  if (type_ == "memory") {
    impl_ = new memory_database(this);
  } else {
    connection_ = dbstring.substr(pos + 3);

    // get driver factory singleton
    database_factory &df = database_factory::instance();

    // try to create database implementation
    impl_ = df.create(type_, this);
  }

  impl_->open(connection_);
}


session::~session()
{
  if (impl_) {
    if (type_ == "memory") {
      delete impl_;
    } else {
      database_factory::instance().destroy(type_, impl_);
    }
  }
}

void session::open()
{
  impl_->open(connection_);
}

bool session::is_open() const
{
  return impl_->is_open();
}

void session::create()
{
  impl_->create();
}

void session::drop()
{
  impl_->drop();
}

void session::close()
{
  impl_->close();
}

bool session::load()
{
  // load sequencer
  impl_->seq()->load();

  prototype_iterator first = ostore_.begin();
  prototype_iterator last = ostore_.end();
  while (first != last) {
    const prototype_node &node = (*first++);
    if (node.abstract) {
      continue;
    }
    impl_->load(node);
  }
  return true;
}

result* session::execute(const std::string &sql)
{
  return impl_->execute(sql);
}

void session::update(const object_base_ptr &optr)
{
  impl_->update(optr.proxy_);
}

void session::remove(object_base_ptr &optr)
{
  transaction tr(*this);
  try {
    tr.begin();
    ostore_.remove(optr);
    tr.commit();
  } catch (std::exception &) {
    return;
  }
}

object_store& session::ostore()
{
  return ostore_;
}

const object_store& session::ostore() const
{
  return ostore_;
}

void session::push_transaction(transaction *tr)
{
  if (!transaction_stack_.empty()) {
    ostore_.unregister_observer(transaction_stack_.top());
  }
  transaction_stack_.push(tr);
  ostore_.register_observer(tr);
}

void session::pop_transaction()
{
  ostore_.unregister_observer(transaction_stack_.top());
  transaction_stack_.pop();
  if (!transaction_stack_.empty()) {
    ostore_.register_observer(transaction_stack_.top());
  }
}

object* session::load(const std::string &/*type*/, int /*id*/)
{
  return 0;
}

void session::begin(transaction &tr)
{
  push_transaction(&tr);
  impl_->prepare();
}

void session::commit(transaction &tr)
{
  impl_->begin();

  transaction::const_iterator first = tr.action_list_.begin();
  transaction::const_iterator last = tr.action_list_.end();
  while (first != last) {
    (*first++)->accept(impl_);
  }

  impl_->commit();
}

void session::rollback()
{
  impl_->rollback();
}

statement* session::create_statement() const
{
  return impl_->create_statement();
}

transaction* session::current_transaction() const
{
  return (transaction_stack_.empty() ? 0 : transaction_stack_.top());
}

const database& session::db() const
{
  return *impl_;
}

database& session::db()
{
  return *impl_;
}

}
