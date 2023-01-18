#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include "matador/object/object_serializer.hpp"
#include "matador/object/object_store.hpp"

using namespace std::placeholders;
using namespace std;

namespace matador {

object_serializer::object_serializer(byte_buffer *buffer)
  : buffer_(buffer) {}

void object_serializer::on_attribute(const char *, char *c, long size)
{
  auto len = size;

  buffer_->append(&len, sizeof(len));
  buffer_->append(c, len);
}

void object_serializer::on_attribute(const char *, std::string &s, long /*size*/)
{
  size_t len = s.size();

  buffer_->append(&len, sizeof(len));
  buffer_->append(s.c_str(), len);
}

void object_serializer::on_attribute(const char *id, date &x, long /*size*/)
{
  int jd(x.julian_date());
  on_attribute(id, jd);
}

void object_serializer::on_attribute(const char *id, time &x, long /*size*/)
{
  struct timeval tv = x.get_timeval();
  on_attribute(id, tv.tv_sec);
  on_attribute(id, tv.tv_usec);
}

void object_serializer::on_belongs_to(const char *id, object_holder &x, cascade_type)
{
  unsigned long oid = x.id();
  on_attribute(id, oid);
}

void object_serializer::on_has_one(const char *id, object_holder &x, cascade_type)
{
  unsigned long oid = x.id();
  on_attribute(id, oid);
}

}
