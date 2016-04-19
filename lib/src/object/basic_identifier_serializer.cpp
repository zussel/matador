/**
 * @author sascha on 4/19/16.
 */

#include "object/basic_identifier_serializer.hpp"

#include "tools/basic_identifier.hpp"

namespace oos {

basic_identifier_serializer::basic_identifier_serializer(byte_buffer &buffer)
  : buffer_(buffer)
{ }

void basic_identifier_serializer::serialize(basic_identifier &x)
{
  restore_ = false;
  x.serialize("", *this);
}

void basic_identifier_serializer::deserialize(basic_identifier &x)
{
  restore_ = true;
  x.serialize("", *this);
}

void basic_identifier_serializer::serialize(const char *, char &)
{
  if (restore_) {

  } else {

  }
}

void basic_identifier_serializer::serialize(const char *, short &)
{

}

void basic_identifier_serializer::serialize(const char *, int &)
{

}

void basic_identifier_serializer::serialize(const char *, long &)
{

}

void basic_identifier_serializer::serialize(const char *, unsigned char &)
{

}

void basic_identifier_serializer::serialize(const char *, unsigned short &)
{

}

void basic_identifier_serializer::serialize(const char *, unsigned int &)
{

}

void basic_identifier_serializer::serialize(const char *, unsigned long &)
{

}

void basic_identifier_serializer::serialize(const char *, bool &)
{

}

void basic_identifier_serializer::serialize(const char *, float &)
{

}

void basic_identifier_serializer::serialize(const char *, double &)
{

}

void basic_identifier_serializer::serialize(const char *, char *, size_t)
{

}

void basic_identifier_serializer::serialize(const char *, std::string &)
{

}

void basic_identifier_serializer::serialize(const char *, oos::varchar_base &)
{

}

void basic_identifier_serializer::serialize(const char *, oos::time &)
{

}

void basic_identifier_serializer::serialize(const char *, oos::date &)
{

}

void basic_identifier_serializer::serialize(const char *, oos::basic_identifier &x) { }

void basic_identifier_serializer::serialize(const char *, oos::identifiable_holder &x, cascade_type) { }

}