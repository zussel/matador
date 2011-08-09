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

#include "object/object_serializer.hpp"
#include "object/object.hpp"

#include <string.h>

namespace oos {

object_serializer::object_serializer()
{}

object_serializer::~object_serializer()
{}

bool object_serializer::serialize(object *o)
{
  buffer_.clear();
  o->write_to(this);
  return true;
}

bool object_serializer::deserialize(object *o)
{
  o->read_from(this);
  return true;
}

void object_serializer::write_char(const char* id, char c)
{
  buffer_.append(&c, sizeof(c));
}

void object_serializer::write_float(const char* id, float f)
{
  buffer_.append(&f, sizeof(f));
}

void object_serializer::write_double(const char* id, double f)
{
  buffer_.append(&f, sizeof(f));
}

void object_serializer::write_int(const char* id, int i)
{
  buffer_.append(&i, sizeof(i));
}

void object_serializer::write_long(const char* id, long l)
{
  buffer_.append(&l, sizeof(l));
}

void object_serializer::write_unsigned(const char* id, unsigned u)
{
  buffer_.append(&u, sizeof(u));
}

void object_serializer::write_bool(const char* id, bool b)
{
  buffer_.append(&b, sizeof(b));
}

void object_serializer::write_charptr(const char* id, const char *c)
{
  size_t len = strlen(c);
  
  buffer_.append(&len, sizeof(len));
  buffer_.append(c, len);
}

void object_serializer::write_string(const char* id, const std::string &s)
{
  size_t len = s.size();
  
  buffer_.append(&len, sizeof(len));
  buffer_.append(s.c_str(), len);
}

void object_serializer::write_date(const char* id, const Date &s)
{
}

void object_serializer::write_time(const char* id, const Time &s)
{
}

void object_serializer::write_object(const char* id, const base_object_ptr &x)
{
}

void object_serializer::write_object_list(const char* id, const object_list_base &x)
{
}

void object_serializer::read_char(const char* id, char &c)
{
  buffer_.release(&c, sizeof(c));
}

void object_serializer::read_float(const char* id, float &f)
{
  buffer_.release(&f, sizeof(f));
}

void object_serializer::read_double(const char* id, double &f)
{
  buffer_.release(&f, sizeof(f));
}

void object_serializer::read_int(const char* id, int &i)
{
  buffer_.release(&i, sizeof(i));
}

void object_serializer::read_long(const char* id, long &l)
{
  buffer_.release(&l, sizeof(l));
}

void object_serializer::read_unsigned(const char* id, unsigned &u)
{
  buffer_.release(&u, sizeof(u));
}

void object_serializer::read_bool(const char* id, bool &b)
{
  buffer_.release(&b, sizeof(b));
}

void object_serializer::read_charptr(const char* id, char *&c)
{
  size_t len = 0;
  buffer_.release(&len, sizeof(len));
  buffer_.release(c, len);
}

void object_serializer::read_string(const char* id, std::string &s)
{
  size_t len = 0;
  buffer_.release(&len, sizeof(len));
  char *str = new char[len];
  buffer_.release(str, len);
  s.assign(str, len);
  delete [] str;
}

void object_serializer::read_date(const char* id, Date &s)
{
}

void object_serializer::read_time(const char* id, Time &s)
{
}

void object_serializer::read_object(const char* id, base_object_ptr &x)
{
}

void object_serializer::read_object_list(const char* id, object_list_base &x)
{
}

}
