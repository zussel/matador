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

#ifndef MEMORY_DATABASE_HPP
#define MEMORY_DATABASE_HPP

#include "database/database.hpp"

namespace oos {

/// @cond OOS_DEV

class memory_database : public database
{
public:
  /**
   * Creates an in memory database within the
   * given session.
   *
   * @param db The corresponding session for the database.
   */
  explicit memory_database(session *db);
  virtual ~memory_database() {}

  virtual bool is_open() const { return true; }
  virtual void create() {}
  virtual void create(const prototype_node&) {}
  virtual void drop() {}
  virtual void drop(const prototype_node&) {}

  virtual void load(const prototype_node&) {}

  virtual void visit(insert_action*) {}
  virtual void visit(update_action*) {}
  virtual void visit(delete_action*) {}

  virtual result* create_result() { return 0; }
  virtual statement* create_statement() { return 0; }
  virtual table* create_table(const prototype_node &) { return 0; }
  virtual void initialize_table(const prototype_node &,
                         std::string &, std::string &) {}
  virtual void prepare_table(const prototype_node &,
                         statement *, statement *,
                         statement *, statement *) {}

  virtual const char* type_string(data_type_t ) const { return 0; }


  virtual unsigned long last_inserted_id() { return 0; }

private:
  virtual void on_open(const std::string &) {}
  virtual void on_close() {}
  virtual result* on_execute(const std::string &) { return 0; }
  virtual void on_begin() {}
  virtual void on_commit() {}
  virtual void on_rollback() {}
};

/// @endcond

}

#endif /* MEMORY_DATABASE_HPP */
