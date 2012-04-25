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

#include "database/memory_database.hpp"
#include "database/database_sequencer.hpp"

namespace oos {
memory_database::memory_database(session *db)
  : database(db, new dummy_database_sequencer)
{}

memory_database::~memory_database()
{}

bool memory_database::is_open() const
{
  return true;
}

void memory_database::create(const prototype_node &node)
{}

void memory_database::load(const prototype_node &node)
{}

void memory_database::execute(const char *sql, result_impl *res)
{}

void memory_database::visit(insert_action*)
{}

void memory_database::visit(update_action*)
{}

void memory_database::visit(delete_action*)
{}

result_impl* memory_database::create_result()
{
  return 0;
}

statement_impl* memory_database::create_statement()
{
  return 0;
}

void memory_database::on_begin()
{}

void memory_database::on_commit()
{}

void memory_database::on_rollback()
{}

}