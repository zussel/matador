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
#include "object/object_store.hpp"

using namespace std::placeholders;
using namespace std;

namespace oos {

object_serializer::~object_serializer()
{}

void object_serializer::serialize(const char *id, char *c, size_t s)
{
  if (restore) {
    std::cout << "restore char pointer " << id;
    size_t len = 0;
    buffer_->release(&len, sizeof(len));
    // TODO: check size of buffer
    buffer_->release(c, len);
    std::cout  << " (value: " << c << ", size: " << s << ")\n" << std::flush;
  } else {
    size_t len = s;

    std::cout << "backup char pointer " << id << " (value: " << c << ", size: " << s << ")\n" << std::flush;
    buffer_->append(&len, sizeof(len));
    buffer_->append(c, len);
  }
}

void object_serializer::serialize(const char *id, std::string &s)
{
  if (restore) {
    std::cout << "restore string " << id;
    size_t len = 0;
    buffer_->release(&len, sizeof(len));
    char *str = new char[len];
    buffer_->release(str, len);
    s.assign(str, len);
    std::cout  << " (value: " << s << ", size: " << len << ")\n" << std::flush;
    delete [] str;
  } else {
    size_t len = s.size();

    std::cout << "backup string " << id << " (value: " << s << ", size: " << s.size() << ")\n" << std::flush;
    buffer_->append(&len, sizeof(len));
    buffer_->append(s.c_str(), len);
  }
}

void object_serializer::serialize(const char *id, date &x)
{
  if (restore) {
    int julian_date(0);
    std::cout << "restore date " << id << " (size: " << sizeof(julian_date) << ")\n" << std::flush;
    buffer_->release(&julian_date, sizeof(julian_date));
    x.set(julian_date);
  } else {
    int jd(x.julian_date());
    std::cout << "backup date " << id << " (size: " << sizeof(jd) << ")\n" << std::flush;
    serialize(id, jd);
  }
}

void object_serializer::serialize(const char *id, time &x)
{
  if (restore) {
    struct timeval tv;
    std::cout << "restore time " << id << " (sec size: " << sizeof(tv.tv_sec) << ", usec size " << sizeof(tv.tv_usec) << ")\n" << std::flush;
    buffer_->release(&tv.tv_sec, sizeof(tv.tv_sec));
    buffer_->release(&tv.tv_usec, sizeof(tv.tv_usec));
    x.set(tv);
  } else {
    struct timeval tv = x.get_timeval();
    std::cout << "backup time " << id << " (sec size: " << sizeof(tv.tv_sec) << ", usec size " << sizeof(tv.tv_usec) << ")\n" << std::flush;
    serialize(id, tv.tv_sec);
    serialize(id, tv.tv_usec);
  }
}

void object_serializer::serialize(const char *id, basic_identifier &x)
{
  if (restore) {
    std::cout << "restore identifier " << id << "\n" << std::flush;
    basic_identifier_serializer_.deserialize(x, *buffer_);
  } else {
    std::cout << "backup identifier " << id << "\n" << std::flush;
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
