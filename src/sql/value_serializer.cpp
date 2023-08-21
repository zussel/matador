#include "matador/sql/value_serializer.hpp"

#include "matador/utils/identifiable_holder.hpp"

namespace matador {
namespace detail {

value_serializer::value_serializer()
: value_identifier_serializer_(*this)
{}

void value_serializer::on_attribute(const char*, char &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, short &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, int &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, long &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, long long &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, unsigned char &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, unsigned short &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, unsigned int &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, unsigned long &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, unsigned long long &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, float &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, double &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, bool &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, char *x, const field_attributes &attr)
{
  if (attr.size() == 0) {
    add_value(x);
  } else {
    add_value(x, attr.size());
  }
}

void value_serializer::on_attribute(const char*, std::string &x, const field_attributes &attr)
{
  if (attr.size() == 0) {
    add_value(x);
  } else {
    add_value(x, attr.size());
  }
}

void value_serializer::on_attribute(const char*, date &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_attribute(const char*, time &x, const field_attributes &/*attr*/)
{
  add_value(x);
}

void value_serializer::on_belongs_to(const char *, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key().serialize(value_identifier_serializer_);
  } else {
    values_->push_back(std::make_shared<null_value>());
  }
}

void value_serializer::on_has_one(const char *, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key().serialize(value_identifier_serializer_);
  } else {
    values_->push_back(std::make_shared<null_value>());
  }
}

void value_identifier_serializer::serialize(null_type_t &, const field_attributes &/*attr*/)
{
//  values_->push_back(std::make_shared<null_value>());
}

}
}
