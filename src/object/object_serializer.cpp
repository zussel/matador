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
#include "object/object_store.hpp"
#include "object/object_ptr.hpp"
#include "object/object_list.hpp"

#include <string.h>

using namespace std::tr1::placeholders;
using namespace std;

namespace oos {

object_serializer::object_serializer()
  : ostore_(NULL)
  , buffer_(NULL)
{}

object_serializer::~object_serializer()
{}

bool object_serializer::serialize(const object *o, byte_buffer &buffer)
{
  buffer_ = &buffer;
  o->write_to(this);
  buffer_ = NULL;
  return true;
}

bool object_serializer::deserialize(object *o, byte_buffer &buffer, object_store *ostore)
{
  ostore_ = ostore;
  buffer_ = &buffer;
  o->read_from(this);
  buffer_ = NULL;
  ostore_ = NULL;
  return true;
}

void object_serializer::write_char(const char*, char c)
{
  buffer_->append(&c, sizeof(c));
}

void object_serializer::write_float(const char*, float f)
{
  buffer_->append(&f, sizeof(f));
}

void object_serializer::write_double(const char*, double f)
{
  buffer_->append(&f, sizeof(f));
}

void object_serializer::write_int(const char*, int i)
{
  buffer_->append(&i, sizeof(i));
}

void object_serializer::write_long(const char*, long l)
{
  buffer_->append(&l, sizeof(l));
}

void object_serializer::write_unsigned(const char*, unsigned u)
{
  buffer_->append(&u, sizeof(u));
}

void object_serializer::write_bool(const char*, bool b)
{
  buffer_->append(&b, sizeof(b));
}

void object_serializer::write_charptr(const char*, const char *c)
{
  size_t len = strlen(c);
  
  buffer_->append(&len, sizeof(len));
  buffer_->append(c, len);
}

void object_serializer::write_string(const char*, const std::string &s)
{
  size_t len = s.size();
  
  buffer_->append(&len, sizeof(len));
  buffer_->append(s.c_str(), len);
}

void object_serializer::write_object(const char*, const object_base_ptr &x)
{
  // write type and id into buffer
  write_long(NULL, x.id());
  write_string(NULL, x.type());
}

void object_serializer::write_object_list(const char*, const object_list_base &x)
{
  // write number of items in list
  // for each item write id and type
  write_unsigned(NULL, x.size());
  x.for_each(std::tr1::bind(&object_serializer::write_object_list_item, this, _1));
}

void object_serializer::read_char(const char*, char &c)
{
  buffer_->release(&c, sizeof(c));
}

void object_serializer::read_float(const char*, float &f)
{
  buffer_->release(&f, sizeof(f));
}

void object_serializer::read_double(const char*, double &f)
{
  buffer_->release(&f, sizeof(f));
}

void object_serializer::read_int(const char*, int &i)
{
  buffer_->release(&i, sizeof(i));
}

void object_serializer::read_long(const char*, long &l)
{
  buffer_->release(&l, sizeof(l));
}

void object_serializer::read_unsigned(const char*, unsigned &u)
{
  buffer_->release(&u, sizeof(u));
}

void object_serializer::read_bool(const char*, bool &b)
{
  buffer_->release(&b, sizeof(b));
}

void object_serializer::read_charptr(const char*, char *&c)
{
  size_t len = 0;
  buffer_->release(&len, sizeof(len));
  buffer_->release(c, len);
}

void object_serializer::read_string(const char*, std::string &s)
{
  size_t len = 0;
  buffer_->release(&len, sizeof(len));
  char *str = new char[len];
  buffer_->release(str, len);
  s.assign(str, len);
  delete [] str;
}

void object_serializer::read_object(const char*, object_base_ptr &x)
{
  /***************
   *
   * extract id and type of object from buffer
   * try to find object on object store
   * if found check type if wrong type throw error
   * else create object and set extracted id
   * insert object into object store
   *
   ***************/
  long id = 0;
  read_long(NULL, id);
  std::string type;
  read_string(NULL, type);

  object_proxy *oproxy = ostore_->find_proxy(id);
  if (!oproxy) {
    oproxy = ostore_->create_proxy(id);
  }
  x.proxy_ = oproxy;
}

void object_serializer::read_object_list(const char*, object_list_base &x)
{
  // get count of backuped list item
  unsigned int s(0);
  read_unsigned(NULL, s);
  x.reset();
  string type;
  long id(0);
  for (unsigned int i = 0; i < s; ++i) {
    read_long(NULL, id);
    read_string(NULL, type);
    object_proxy *oproxy = ostore_->find_proxy(id);
    if (!oproxy) {
      oproxy = ostore_->create_proxy(id);
    }
    x.push_back_proxy(oproxy);
  }
}

void object_serializer::write_object_list_item(const object *o)
{
  write_long(NULL, o->id());
  write_string(NULL, o->object_type());
}

}
