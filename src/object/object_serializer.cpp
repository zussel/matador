#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include "matador/object/object_serializer.hpp"
#include "matador/object/object_store.hpp"

using namespace std::placeholders;
using namespace std;

namespace matador {

object_serializer::object_serializer(byte_buffer *buffer)
  : buffer_(buffer) {}

void object_serializer::serialize(const char *, char *c, size_t s)
{
  size_t len = s;

  buffer_->append(&len, sizeof(len));
  buffer_->append(c, len);
}

void object_serializer::serialize(const char *, std::string &s, size_t)
{
  size_t len = s.size();

  buffer_->append(&len, sizeof(len));
  buffer_->append(s.c_str(), len);
}

void object_serializer::serialize(const char *, std::string &s)
{
  size_t len = s.size();

  buffer_->append(&len, sizeof(len));
  buffer_->append(s.c_str(), len);
}

void object_serializer::serialize(const char *id, date &x)
{
  int jd(x.julian_date());
  serialize(id, jd);
}

void object_serializer::serialize(const char *id, time &x)
{
  struct timeval tv = x.get_timeval();
  serialize(id, tv.tv_sec);
  serialize(id, tv.tv_usec);
}

void object_serializer::serialize(const char *, basic_identifier &x)
{
  basic_identifier_serializer_.serialize(x, *buffer_);
}

void object_serializer::serialize(const char* id, object_holder &x, cascade_type)
{
  unsigned long oid = x.id();
  serialize(id, oid);
}

}
