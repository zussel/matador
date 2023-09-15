#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include "matador/object/object_serializer.hpp"
#include "matador/object/object_store.hpp"

using namespace std::placeholders;
using namespace std;

namespace matador {

object_serializer::object_serializer(byte_buffer *buffer)
  : buffer_(buffer) {}

void object_serializer::on_attribute(const char *, char *x, const field_attributes &/*attr*/)
{
  size_t len = strlen(x);

  buffer_->append(&len, sizeof(len));
  buffer_->append(x, len);
}

void object_serializer::on_attribute(const char *, std::string &x, const field_attributes &/*attr*/)
{
  size_t len = x.size();

  buffer_->append(&len, sizeof(len));
  buffer_->append(x.c_str(), len);
}

void object_serializer::on_attribute(const char *id, date &x, const field_attributes &/*attr*/)
{
  int jd(x.julian_date());
  on_attribute(id, jd);
}

void object_serializer::on_attribute(const char *id, time &x, const field_attributes &/*attr*/)
{
  time_info ti = x.get_time_info();
  on_attribute(id, ti.seconds_since_epoch);
  on_attribute(id, ti.milliseconds);
}

void object_serializer::on_belongs_to(const char *id, object_holder &x, cascade_type cascade)
{
  on_foreign_object(id, x, cascade);
}

void object_serializer::on_has_one(const char *id, object_holder &x, cascade_type cascade)
{
  on_foreign_object(id, x, cascade);
}

void object_serializer::on_foreign_object(const char *id, object_holder &x, cascade_type /*cascade*/)
{
  auto oid = x.id();
  on_attribute(id, oid);
}

}
