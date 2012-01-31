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

#include "database/database_impl.hpp"
#include "database/transaction.hpp"
#include "database/statement.hpp"
#include "database/statement_binder.hpp"

namespace oos {

database_impl::database_impl()
  : db_(0)
{}

database_impl::~database_impl()
{}

bool database_impl::store_statement(const std::string &id, database_impl::statement_impl_ptr stmt)
{
  return statement_impl_map_.insert(std::make_pair(id, stmt)).second;
}

database_impl::statement_impl_ptr database_impl::find_statement(const std::string &id) const
{
  statement_impl_map_t::const_iterator i = statement_impl_map_.find(id);
  if (i != statement_impl_map_.end()) {
    return i->second;
  } else {
    return statement_impl_ptr();
  }
}

void database_impl::commit(const transaction::insert_action_map_t &insert_actions, const transaction::action_list_t &modify_actions)
{
  /****************
   * 
   * Pop transaction from stack
   * and execute all actions
   * change state to comitted
   * 
   ****************/

  // execute begin statement
  statement_binder binder;

  transaction::insert_action_map_t::const_iterator ifirst = insert_actions.begin();
  transaction::insert_action_map_t::const_iterator ilast = insert_actions.end();
  while (ifirst != ilast) {

    statement_impl_ptr stmt = find_statement(ifirst->first + "_INSERT");
      
    transaction::const_iterator first = ifirst->second.begin();
    transaction::const_iterator last = ifirst->second.end();
    while (first != last) {
      std::auto_ptr<action> a(*first++);
      
      binder.bind(stmt.get(), a->obj(), false);
      
      stmt->step();
//      a->accept(tr_.db_.impl_);
//      ifirst->second.pop_front();
    }
    ++ifirst;
  }
//  insert_actions.clear();
  transaction::const_iterator first = modify_actions.begin();
  transaction::const_iterator last = modify_actions.end();
  while (first != last) {
    (*first++)->accept(this);
  }

  // execute commit statement
}

void database_impl::initialize(database *db)
{
  db_ = db;
}

}
