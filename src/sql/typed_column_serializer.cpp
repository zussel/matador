#include "matador/sql/typed_column_serializer.hpp"
#include "matador/sql/columns.hpp"

#include "matador/utils/identifiable_holder.hpp"

namespace matador {
namespace detail {

typed_column_serializer::typed_column_serializer()
  : column_identifier_serializer_(*this)
{}

void typed_column_serializer::on_primary_key(const char *id, std::string &pk, size_t size)
{
  on_attribute(id, pk, { size, constraints::PRIMARY_KEY });
}

void typed_column_serializer::on_revision(const char *id, unsigned long long int &rev)
{
  on_attribute(id, rev);
}

void typed_column_serializer::on_attribute(const char *id, char&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_char, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, short&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_short, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, int&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_int, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, long&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_long, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, long long&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_long_long, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, unsigned char&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_unsigned_char, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, unsigned short&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_unsigned_short, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, unsigned int&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_unsigned_int, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, unsigned long&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_unsigned_long, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, unsigned long long&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_unsigned_long_long, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, float&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_float, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, double&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_double, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, bool&, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_bool, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, char *, const field_attributes &attr)
{
  const auto dt = attr.size() == 0 ? data_type::type_text : data_type::type_varchar;
  cols_->push_back(make_column(id, dt, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, std::string &, const field_attributes &attr)
{
  const auto dt = attr.size() == 0 ? data_type::type_text : data_type::type_varchar;
  cols_->push_back(make_column(id, dt, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, date &, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_date, index_++, attr));
}

void typed_column_serializer::on_attribute(const char *id, time &, const field_attributes &attr)
{
  cols_->push_back(make_column(id, data_type::type_time, index_++, attr));
}

void typed_column_serializer::on_belongs_to(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    column_identifier_serializer_.make_typed_column(id, x.primary_key());
  } else {
    auto pk(x.create_identifier());
    column_identifier_serializer_.make_typed_column(id, pk);
  }
}

void typed_column_serializer::on_has_one(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    column_identifier_serializer_.make_typed_column(id, x.primary_key());
  } else {
    auto pk(x.create_identifier());
    column_identifier_serializer_.make_typed_column(id, pk);
  }
}

void typed_column_identifier_serializer::make_typed_column(const char *id, identifier &pk)
{
  id_ = id;
  pk.serialize(*this);
  id_ = nullptr;
}

}
}
