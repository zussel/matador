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

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
  #else
    #define OOS_API __declspec(dllimport)
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_API
#endif

#include "database/database.hpp"

namespace oos {

class OOS_API memory_database : public database
{
public:
  explicit memory_database(session *db);
  virtual ~memory_database();

  virtual bool is_open() const;
  virtual void create(const prototype_node &node);

  virtual void load(const prototype_node &node);

  virtual void execute(const char *sql, result_impl *res = 0);

  virtual void visit(insert_action*);
  virtual void visit(update_action*);
  virtual void visit(delete_action*);

  virtual result_impl* create_result();
  virtual statement_impl* create_statement();

private:
  virtual void on_begin();
  virtual void on_commit();
  virtual void on_rollback();
};

}

#endif /* MEMORY_DATABASE_HPP */
