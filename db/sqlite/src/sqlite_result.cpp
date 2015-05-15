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

#include <object/primary_key.hpp>
#include "sqlite_result.hpp"

namespace oos {

namespace sqlite {

sqlite_result::sqlite_result()
  : pos_(-1)
{}

sqlite_result::~sqlite_result()
{
  while (!rows_.empty()) {
    row *r = rows_.back();
    delete r;
    rows_.pop_back();
  }
}

const char* sqlite_result::column(sqlite_result::size_type c) const
{
  return rows_.at(pos_)->at<std::string>(c).c_str();
}

bool sqlite_result::fetch()
{
  return ++pos_ < rows_.size();
}

bool sqlite_result::fetch(object *)
{
  return false;
}

sqlite_result::size_type sqlite_result::affected_rows() const
{
  return 0;
}

sqlite_result::size_type sqlite_result::result_rows() const
{
  return rows_.size();
}

sqlite_result::size_type sqlite_result::fields() const
{
  return 0;
}

int sqlite_result::transform_index(int index) const
{
  return index;
}

void sqlite_result::push_back(row *r)
{
  rows_.push_back(r);
}

void sqlite_result::read(const char *id, char &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, short &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, int &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, long &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, unsigned char &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, unsigned short &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, unsigned int &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, unsigned long &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, bool &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, float &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, double &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char */*id*/, char */*x*/, int /*s*/)
{
}

void sqlite_result::read(const char *id, varchar_base &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, std::string &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, oos::date &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char *id, oos::time &x)
{
  read_column(id, x);
}

void sqlite_result::read(const char */*id*/, object_base_ptr &/*x*/)
{
}

void sqlite_result::read(const char */*id*/, object_container &/*x*/)
{
}

void sqlite_result::read(const char *id, primary_key_base &x)
{
  x.deserialize(id, *this);
}

void sqlite_result::read_column(char const *, oos::date &/*x*/)
{

}

void sqlite_result::read_column(char const *, oos::time &/*x*/)
{

}

}

}
