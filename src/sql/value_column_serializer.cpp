#include "matador/sql/value_column_serializer.hpp"
#include "matador/sql/columns.hpp"

#include "matador/utils/identifiable_holder.hpp"

namespace matador {
namespace detail {
value_column_identifier_serializer &value_column_identifier_serializer::for_column(const char *id) {
  id_ = id;
  return *this;
}

value_column_serializer::value_column_serializer()
: value_column_identifier_serializer_(*this)
{}

void value_column_serializer::on_primary_key(const char *id, std::string &pk, size_t size)
{
  on_attribute(id, pk, { size, constraints::PRIMARY_KEY });
}

void value_column_serializer::on_revision(const char *id, unsigned long long int &rev)
{
  on_attribute(id, rev);
}

void value_column_serializer::on_attribute(const char *id, char *x, const field_attributes &attr)
{
  cols_->push_back(make_column(id, std::string(x), attr));
}

void value_column_serializer::on_belongs_to(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key().serialize(value_column_identifier_serializer_.for_column(id));
  } else {
    x.create_identifier().serialize(value_column_identifier_serializer_.for_column(id));
  }
}

void value_column_serializer::on_has_one(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key().serialize(value_column_identifier_serializer_.for_column(id));
  } else {
    x.create_identifier().serialize(value_column_identifier_serializer_.for_column(id));
  }
}

}
}
