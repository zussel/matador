//
// Created by sascha on 06.03.16.
//

#include "sql/value_serializer.hpp"

#include "tools/identifiable_holder.hpp"
#include "tools/basic_identifier.hpp"

namespace oos {
namespace detail {

value_serializer::value_serializer(oos::sql &d)
  : sql_(d)
{}

void value_serializer::serialize(const char*, char &x)
{
  values_->push_back(std::make_shared<value<char>>(x));
}

void value_serializer::serialize(const char*, short &x)
{
  values_->push_back(std::make_shared<value<short>>(x));
}

void value_serializer::serialize(const char*, int &x)
{
  values_->push_back(std::make_shared<value<int>>(x));
}

void value_serializer::serialize(const char*, long &x)
{
  values_->push_back(std::make_shared<value<long>>(x));
}

void value_serializer::serialize(const char*, unsigned char &x)
{
  values_->push_back(std::make_shared<value<unsigned char>>(x));
}

void value_serializer::serialize(const char*, unsigned short &x)
{
  values_->push_back(std::make_shared<value<unsigned short>>(x));
}

void value_serializer::serialize(const char*, unsigned int &x)
{
  values_->push_back(std::make_shared<value<unsigned int>>(x));
}

void value_serializer::serialize(const char*, unsigned long &x)
{
  values_->push_back(std::make_shared<value<unsigned long>>(x));
}

void value_serializer::serialize(const char*, float &x)
{
  values_->push_back(std::make_shared<value<float>>(x));
}

void value_serializer::serialize(const char*, double &x)
{
  values_->push_back(std::make_shared<value<double>>(x));
}

void value_serializer::serialize(const char*, bool &x)
{
  values_->push_back(std::make_shared<value<bool>>(x));
}

void value_serializer::serialize(const char*, char *x, size_t len)
{
  values_->push_back(std::make_shared<value<char*>>(x, len));
}

void value_serializer::serialize(const char*, varchar_base &x)
{
  values_->push_back(std::make_shared<value<varchar_base>>(x));
}

void value_serializer::serialize(const char*, std::string  &x)
{
  values_->push_back(std::make_shared<value<std::string>>(x));
}

void value_serializer::serialize(const char*, date &x)
{
  values_->push_back(std::make_shared<value<date>>(x));
}

void value_serializer::serialize(const char*, time &x)
{
  values_->push_back(std::make_shared<value<time>>(x));
}

void value_serializer::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key()->serialize(id, *this);
  } else {
    values_->push_back(std::make_shared<null_value>());
  }
}

void value_serializer::serialize(const char *id, basic_identifier &x)
{
  x.serialize(id, *this);
}

}
}
