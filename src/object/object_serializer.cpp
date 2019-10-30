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

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include "matador/object/object_serializer.hpp"
#include "matador/object/object_store.hpp"

using namespace std::placeholders;
using namespace std;

namespace matador {

void object_serializer::serialize(const char *, char *c, size_t s)
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

void object_serializer::serialize(const char *, std::string &s, size_t)
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

void object_serializer::serialize(const char *, std::string &s)
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
    struct timeval tv{};
    buffer_->release(&tv.tv_sec, sizeof(tv.tv_sec));
    buffer_->release(&tv.tv_usec, sizeof(tv.tv_usec));
    x.set(tv);
  } else {
    struct timeval tv = x.get_timeval();
    serialize(id, tv.tv_sec);
    serialize(id, tv.tv_usec);
  }
}

void object_serializer::serialize(const char *, basic_identifier &x)
{
  if (restore) {
    basic_identifier_serializer_.deserialize(x, *buffer_);
  } else {
    basic_identifier_serializer_.serialize(x, *buffer_);
  }
}

object_proxy *object_serializer::find_proxy(unsigned long oid)
{
  return ostore_->find_proxy(oid);
}

void object_serializer::insert_proxy(object_proxy *proxy)
{
  ostore_->insert_proxy(proxy);
}

}
