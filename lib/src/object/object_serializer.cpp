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

#include "tools/date.hpp"
#include "tools/time.hpp"
#include "tools/varchar.hpp"

#include "object/object_serializer.hpp"
#include "object/prototype_node.hpp"
#include "object/object_container.hpp"


using namespace std::placeholders;
using namespace std;

namespace oos {

object_serializer::~object_serializer()
{}

void object_serializer::serialize(const char*, char *c, size_t s)
{
  if (restore) {
    size_t len = 0;
    buffer_->release(&len, sizeof(len));
    // TODO: check size of buffer
    buffer_->release(c, len);
  } else {
    size_t len = s;

    buffer_->append(&len, sizeof(len));
    buffer_->append(c, len);
  }
}

void object_serializer::serialize(const char*, std::string &s)
{
  if (restore) {
    size_t len = 0;
    buffer_->release(&len, sizeof(len));
    char *str = new char[len];
    buffer_->release(str, len);
    s.assign(str, len);
    delete [] str;
  } else {
    size_t len = s.size();

    buffer_->append(&len, sizeof(len));
    buffer_->append(s.c_str(), len);
  }
}

void object_serializer::serialize(const char *id, date &x)
{
  if (restore) {
    int julian_date(0);
    buffer_->release(&julian_date, sizeof(julian_date));
    x.set(julian_date);
  } else {
    int jd(x.julian_date());
    serialize(id, jd);
  }
}

void object_serializer::serialize(const char *id, time &x)
{
  if (restore) {
    struct timeval tv;
    buffer_->release(&tv.tv_sec, sizeof(tv.tv_sec));
    buffer_->release(&tv.tv_usec, sizeof(tv.tv_usec));
    x.set(tv);
  } else {
    struct timeval tv = x.get_timeval();
    serialize(id, tv.tv_sec);
    serialize(id, tv.tv_usec);
  }
}

//void object_serializer::serialize(const char *id, const object_container &x)
//{
  // write number of items in list
  // for each item write id and type
//  serialize(id, (unsigned long)x.size());
//  x.for_each(std::bind(&object_serializer::write_object_container_item, this, _1));
//}

//void object_serializer::serialize(const char */*id*/, object_container &/*x*/)
//{
  // get count of backuped list item
//  object_container::size_type s(0);
  // Todo: correct implementation
//  unsigned long s(0);
//  serialize(id, s);
//  x.reset();
//  string type;
//  unsigned long oid(0);
//  for (unsigned int i = 0; i < s; ++i) {
//    serialize(id, oid);
//    serialize(id, type);
//    object_proxy *oproxy = ostore_->find_proxy(oid);
//    if (!oproxy) {
//      oproxy = ostore_->create_proxy(oid);
//    }
//    x.append_proxy(oproxy);
//  }
//}

void object_serializer::write_object_container_item(const object_proxy *proxy)
{
  unsigned long id(proxy->id());
  serialize(nullptr, id);
  std::string type(proxy->node()->type());
  serialize(nullptr, type);
}

}
