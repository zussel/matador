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

#include <tools/date.hpp>
#include <tools/time.hpp>
#include "object/object_serializer.hpp"
#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/object_list.hpp"

#include "tools/byte_buffer.hpp"

using namespace std::placeholders;
using namespace std;

namespace oos {

object_serializer::~object_serializer()
{}

bool object_serializer::serialize(const object *o, byte_buffer *buffer)
{
  buffer_ = buffer;
  o->serialize(*this);
  buffer_ = NULL;
  return true;
}

bool object_serializer::deserialize(object *o, byte_buffer *buffer, object_store *ostore)
{
  ostore_ = ostore;
  buffer_ = buffer;
  o->deserialize(*this);
  buffer_ = NULL;
  ostore_ = NULL;
  return true;
}

void object_serializer::write_value(const char*, const char *c, int s)
{
  size_t len = s;
  
  buffer_->append(&len, sizeof(len));
  buffer_->append(c, len);
}

void object_serializer::write_value(const char*, const std::string &s)
{
  size_t len = s.size();
  
  buffer_->append(&len, sizeof(len));
  buffer_->append(s.c_str(), len);
}

void object_serializer::write_value(const char*, const varchar_base &s)
{
  size_t len = s.size();
  
  buffer_->append(&len, sizeof(len));
  buffer_->append(s.str().c_str(), len);
}

void object_serializer::write_value(const char *id, const date &x)
{
  write_value(id, x.julian_date());
}

void object_serializer::write_value(const char *id, const time &x)
{
  struct timeval tv = x.get_timeval();
  write_value(id, tv.tv_sec);
  write_value(id, tv.tv_usec);
}

void object_serializer::write_value(const char*, const object_base_ptr &x)
{
  // write type and id into buffer
  write(0, x.id());
  write(0, x.type(), strlen(x.type()));
}

void object_serializer::write_value(const char*, const object_container &x)
{
  // write number of items in list
  // for each item write id and type
  write(0, x.size());
  x.for_each(std::bind(&object_serializer::write_object_container_item, this, _1));
}

void object_serializer::write_value(const char *id, const primary_key_base &x)
{
  x.serialize(id, *this);
}

void object_serializer::read_value(const char*, char *&c, int )
{
  size_t len = 0;
  buffer_->release(&len, sizeof(len));
  // TODO: check size of buffer
  buffer_->release(c, len);
}

void object_serializer::read_value(const char*, std::string &s)
{
  size_t len = 0;
  buffer_->release(&len, sizeof(len));
  char *str = new char[len];
  buffer_->release(str, len);
  s.assign(str, len);
  delete [] str;
}

void object_serializer::read_value(const char*, varchar_base &s)
{
  size_t len = 0;
  buffer_->release(&len, sizeof(len));
  char *str = new char[len];
  buffer_->release(str, len);
  s.assign(str, len);
  delete [] str;
}

void object_serializer::read_value(const char *, date &x)
{
  int julian_date(0);
  buffer_->release(&julian_date, sizeof(julian_date));
  x.set(julian_date);
}

void object_serializer::read_value(const char *, time &x)
{
  struct timeval tv;
  buffer_->release(&tv.tv_sec, sizeof(tv.tv_sec));
  buffer_->release(&tv.tv_usec, sizeof(tv.tv_usec));
  x.set(tv);
}

void object_serializer::read_value(const char*, object_base_ptr &x)
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
  read(0, id);
  std::string type;
  read(0, type);

  if (id > 0) {
    object_proxy *oproxy = ostore_->find_proxy(id);
    if (!oproxy) {
      oproxy = ostore_->create_proxy(id);
    }
    x.reset(oproxy);
  } else {
    x.proxy_ = new object_proxy(id, nullptr);
  }
}

void object_serializer::read_value(const char*, object_container &x)
{
  // get count of backuped list item
  object_container::size_type s(0);
  read(0, s);
  x.reset();
  string type;
  long id(0);
  for (unsigned int i = 0; i < s; ++i) {
    read(0, id);
    read(0, type);
    object_proxy *oproxy = ostore_->find_proxy(id);
    if (!oproxy) {
      oproxy = ostore_->create_proxy(id);
    }
    x.append_proxy(oproxy);
  }
}

void object_serializer::read_value(const char *id, primary_key_base &x)
{
  x.deserialize(id, *this);
}

void object_serializer::write_object_container_item(const object_proxy *proxy)
{
  write(0, proxy->id());
  write(0, proxy->node->type);
}

}
