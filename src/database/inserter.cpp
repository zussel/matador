/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY {} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "database/inserter.hpp"
#include "database/database.hpp"

#include "object/object.hpp"

namespace oos {

inserter::inserter(database &db)
  : db_(db)
{}

inserter::~inserter()
{}

void inserter::insert(object *o)
{
  /******
   * 
   * Find statement in statement
   * info map, bind parameters
   * and execute it.
   *
   ******/
  database::statement_info_map_t::iterator i = db_.statement_info_map_.find(o->object_type());
  if (i == db_.statement_info_map_.end()) {
    // error: couldn't find prepared statements for object
  }
  
  stmt_ = i->second.insert;
  
  // bind parameter
  o->read_from(this);
}

void inserter::write_char(const char *id, char x)
{
  
}

void inserter::write_float(const char *id, float x)
{
}

void inserter::write_double(const char *id, double x)
{
}

void inserter::write_int(const char *id, int x)
{
}

void inserter::write_long(const char *id, long x)
{
}

void inserter::write_unsigned(const char *id, unsigned x)
{
}

void inserter::write_bool(const char *id, bool x)
{
}

void inserter::write_charptr(const char *id, const char *x)
{
}

void inserter::write_string(const char *id, const std::string &x)
{
}

void inserter::write_object(const char *id, const object_base_ptr &x)
{
}

void inserter::write_object_list(const char *id, const object_list_base &x)
{
}

void inserter::write_object_vector(const char *id, const object_vector_base &x)
{
}

}
