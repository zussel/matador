#include "matador/sql/value_serializer.hpp"

#include "matador/utils/identifiable_holder.hpp"

namespace matador {
namespace detail {

value_serializer::value_serializer()
: value_identifier_serializer_(*this)
{}

void value_serializer::on_primary_key(const char *id, std::string &pk, size_t size)
{
  on_attribute(id, pk, { size, constraints::PRIMARY_KEY });
}

void value_serializer::on_revision(const char *id, unsigned long long int &rev)
{
  on_attribute(id, rev);
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

void value_serializer::on_belongs_to(const char *, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key().serialize(value_identifier_serializer_);
  } else {
    x.create_identifier().serialize(value_identifier_serializer_);
  }
}

void value_serializer::on_has_one(const char *, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key().serialize(value_identifier_serializer_);
  } else {
    x.create_identifier().serialize(value_identifier_serializer_);
  }
}

void value_identifier_serializer::serialize(null_type_t &, const field_attributes &/*attr*/)
{
//  values_->push_back(std::make_shared<null_value>());
}

}
}
