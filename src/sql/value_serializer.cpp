#include "matador/sql/value_serializer.hpp"

#include "matador/utils/identifiable_holder.hpp"
#include "matador/utils/basic_identifier.hpp"

namespace matador {
namespace detail {

void value_serializer::on_attribute(const char*, char &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, short &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, int &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, long &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, long long &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, unsigned char &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, unsigned short &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, unsigned int &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, unsigned long &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, unsigned long long &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, float &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, double &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, bool &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, char *x, size_t len)
{
  values_->push_back(std::make_shared<value>(x, len));
}

void value_serializer::on_attribute(const char*, std::string &x, size_t len)
{
  values_->push_back(std::make_shared<value>(x, len));
}

void value_serializer::on_attribute(const char*, std::string  &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, date &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_attribute(const char*, time &x)
{
  values_->push_back(std::make_shared<value>(x));
}

void value_serializer::on_belongs_to(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
//    x.primary_key()->serialize(id, *this);
  } else {
    values_->push_back(std::make_shared<null_value>());
  }
}

void value_serializer::on_has_one(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
//    x.primary_key()->serialize(id, *this);
  } else {
    values_->push_back(std::make_shared<null_value>());
  }
}

void value_serializer::on_primary_key(const char *id, basic_identifier &x)
{
//  x.serialize(id, *this);
}

}
}
