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

#include "database/reader.hpp"
#include "database/statement_helper.hpp"

namespace oos {

reader::reader(database &db, object *o, const std::string name)
  : db_(db)
{
  statement_helper helper;
  std::string sql = helper.create(o, name, statement_helper::SELECT);

  //statement stmt(db, sql);

}

reader::~reader()
{}

bool reader::read()
{

  return true;
}

void reader::read_char(const char *id, char &x)
{
}

void reader::read_float(const char *id, float &x)
{
}

void reader::read_double(const char *id, double &x)
{
}

void reader::read_int(const char *id, int &x)
{
}

void reader::read_long(const char *id, long &x)
{
}

void reader::read_unsigned(const char *id, unsigned &x)
{
}

void reader::read_bool(const char *id, bool &x)
{
}

void reader::read_charptr(const char *id, char* &x)
{
}

void reader::read_string(const char *id, std::string &x)
{
}

void reader::read_object(const char *id, object_base_ptr &x)
{
}

void reader::read_object_list(const char *id, object_list_base &x)
{
}

void reader::read_object_vector(const char *id, object_vector_base &x)
{
}

}
