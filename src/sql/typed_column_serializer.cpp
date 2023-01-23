#include "matador/sql/typed_column_serializer.hpp"

#include "matador/utils/identifiable_holder.hpp"

namespace matador {
namespace detail {

template <>
std::shared_ptr<column> make_column<typed_column>(const char *id, data_type t, size_t index)
{
  return std::make_shared<typed_column>(id, t, index, false);
}

template <>
std::shared_ptr<column> make_column<typed_identifier_column>(const char *id, data_type t, size_t index)
{
  return std::make_shared<typed_identifier_column>(id, t, index, false);
}

template <>
std::shared_ptr<column> make_varchar_column<typed_varchar_column>(const char *id, size_t s, data_type t, size_t index)
{
  return std::make_shared<typed_varchar_column>(id, s, t, index, false);
}

template <>
std::shared_ptr<column> make_varchar_column<identifier_varchar_column>(const char *id, size_t s, data_type t, size_t index)
{
  return std::make_shared<identifier_varchar_column>(id, s, t, index, false);
}

typed_column_serializer::typed_column_serializer()
  : create_column_func_(make_column<typed_column>)
  , create_varchar_column_func_(make_varchar_column<typed_varchar_column>)
  , column_identifier_serializer_(*this)
{}

void typed_column_serializer::on_attribute(const char *id, char&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_char, index_++));
}

void typed_column_serializer::on_attribute(const char *id, short&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_short, index_++));
}

void typed_column_serializer::on_attribute(const char *id, int&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_int, index_++));
}

void typed_column_serializer::on_attribute(const char *id, long&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_long, index_++));
}

void typed_column_serializer::on_attribute(const char *id, long long&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_long_long, index_++));
}

void typed_column_serializer::on_attribute(const char *id, unsigned char&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_char, index_++));
}

void typed_column_serializer::on_attribute(const char *id, unsigned short&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_unsigned_short, index_++));
}

void typed_column_serializer::on_attribute(const char *id, unsigned int&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_unsigned_int, index_++));
}

void typed_column_serializer::on_attribute(const char *id, unsigned long&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_unsigned_long, index_++));
}

void typed_column_serializer::on_attribute(const char *id, unsigned long long&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_unsigned_long_long, index_++));
}

void typed_column_serializer::on_attribute(const char *id, float&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_float, index_++));
}

void typed_column_serializer::on_attribute(const char *id, double&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_double, index_++));
}

void typed_column_serializer::on_attribute(const char *id, bool&, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_bool, index_++));
}

void typed_column_serializer::on_attribute(const char *id, char *, long size)
{
  if (size < 0) {
    cols_->push_back(create_column_func_(id, data_type::type_text, index_++));
  } else {
    cols_->push_back(create_varchar_column_func_(id, size, data_type::type_varchar, index_++));
  }
}

void typed_column_serializer::on_attribute(const char *id, std::string &, long size)
{
  if (size < 0) {
    cols_->push_back(create_column_func_(id, data_type::type_text, index_++));
  } else {
    cols_->push_back(create_varchar_column_func_(id, size, data_type::type_varchar, index_++));
  }
}

void typed_column_serializer::on_attribute(const char *id, date &, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_date, index_++));
}

void typed_column_serializer::on_attribute(const char *id, time &, long /*size*/)
{
  cols_->push_back(create_column_func_(id, data_type::type_time, index_++));
}

void typed_column_serializer::on_belongs_to(const char *, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key().serialize(column_identifier_serializer_);
  } else {
    auto pk(x.create_identifier());
    pk.serialize(column_identifier_serializer_);
  }
}

void typed_column_serializer::on_has_one(const char *, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key().serialize(column_identifier_serializer_);
  } else {
    auto pk(x.create_identifier());
    pk.serialize(column_identifier_serializer_);
  }
}

void typed_column_identifier_serializer::serialize(null_type_t &)
{

}

}
}
