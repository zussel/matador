/**
 * @author sascha on 4/19/16.
 */

#include "oos/utils/basic_identifier_serializer.hpp"

#include "oos/utils/basic_identifier.hpp"
#include "oos/utils/time.hpp"

namespace oos {

basic_identifier_serializer::basic_identifier_serializer()
{ }

void basic_identifier_serializer::serialize(basic_identifier &x, byte_buffer &buffer)
{
  restore_ = false;
  buffer_ = &buffer;
  basic_identifier_.reset(x.clone());
  x.serialize("", *this);
}

void basic_identifier_serializer::deserialize(basic_identifier &x, byte_buffer &buffer)
{
  if (!basic_identifier_  || !basic_identifier_->is_same_type(x)) {
    throw std::logic_error("invalid identifier type");
  }
  restore_ = true;
  buffer_ = &buffer;
  x.serialize("", *this);
  basic_identifier_.reset();
}

void basic_identifier_serializer::serialize(const char *, char &x)
{
  serialize_value(x);
}

void basic_identifier_serializer::serialize(const char *, short &x)
{
  serialize_value(x);
}

void basic_identifier_serializer::serialize(const char *, int &x)
{
  serialize_value(x);
}

void basic_identifier_serializer::serialize(const char *, long &x)
{
  serialize_value(x);
}

void basic_identifier_serializer::serialize(const char *, unsigned char &x)
{
  serialize_value(x);
}

void basic_identifier_serializer::serialize(const char *, unsigned short &x)
{
  serialize_value(x);
}

void basic_identifier_serializer::serialize(const char *, unsigned int &x)
{
  serialize_value(x);
}

void basic_identifier_serializer::serialize(const char *, unsigned long &x)
{
  serialize_value(x);
}

void basic_identifier_serializer::serialize(const char *, bool &x)
{
  serialize_value(x);
}

void basic_identifier_serializer::serialize(const char *, float &x)
{
  serialize_value(x);
}

void basic_identifier_serializer::serialize(const char *, double &x)
{
  serialize_value(x);
}

void basic_identifier_serializer::serialize(const char *, char *x, size_t size)
{
  if (restore_) {
    size_t len = 0;
    buffer_->release(&len, sizeof(len));
    // TODO: check size of buffer
    buffer_->release(x, len);
  } else {
    size_t len = size;

    buffer_->append(&len, sizeof(len));
    buffer_->append(x, len);
  }
}

void basic_identifier_serializer::serialize(const char *, std::string &x)
{
  if (restore_) {
    size_t len = 0;
    buffer_->release(&len, sizeof(len));
    char *str = new char[len];
    buffer_->release(str, len);
    x.assign(str, len);
    delete [] str;
  } else {
    size_t len = x.size();

    buffer_->append(&len, sizeof(len));
    buffer_->append(x.c_str(), len);
  }
}

void basic_identifier_serializer::serialize(const char *id, oos::varchar_base &x)
{
  std::string value;
  if (restore_) {
    serialize(id, value);
    x = value;
  } else {
    value = x.str();
    serialize(id, value);
  }
}

void basic_identifier_serializer::serialize(const char *id, oos::time &x)
{
  if (restore_) {
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

void basic_identifier_serializer::serialize(const char *id, oos::date &x)
{
  if (restore_) {
    int julian_date(0);
    buffer_->release(&julian_date, sizeof(julian_date));
    x.set(julian_date);
  } else {
    int jd(x.julian_date());
    serialize(id, jd);
  }
}

void basic_identifier_serializer::serialize(const char *, oos::basic_identifier &) { }

void basic_identifier_serializer::serialize(const char *, oos::identifiable_holder &, cascade_type) { }

}