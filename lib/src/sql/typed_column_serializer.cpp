//
// Created by sascha on 2/25/16.
//
#include "sql/typed_column_serializer.hpp"

#include "tools/identifiable_holder.hpp"
#include "tools/basic_identifier.hpp"
#include "tools/varchar.hpp"

namespace oos {
namespace detail {


template < class T >
std::shared_ptr<column> make_column(const char *id, data_type t, size_t index);

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

template < class T >
std::shared_ptr<column> make_varchar_column(const char *id, size_t s, data_type t, size_t index);

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
{}

void typed_column_serializer::serialize(const char *id, char&)
{
  cols_->push_back(create_column_func_(id, data_type::type_char, index_++));
}

void typed_column_serializer::serialize(const char *id, short&)
{
  cols_->push_back(create_column_func_(id, data_type::type_short, index_++));
}

void typed_column_serializer::serialize(const char *id, int&)
{
  cols_->push_back(create_column_func_(id, data_type::type_int, index_++));
}

void typed_column_serializer::serialize(const char *id, long&)
{
  cols_->push_back(create_column_func_(id, data_type::type_long, index_++));
}

void typed_column_serializer::serialize(const char *id, unsigned char&)
{
  cols_->push_back(create_column_func_(id, data_type::type_unsigned_char, index_++));
}

void typed_column_serializer::serialize(const char *id, unsigned short&)
{
  cols_->push_back(create_column_func_(id, data_type::type_unsigned_short, index_++));
}

void typed_column_serializer::serialize(const char *id, unsigned int&)
{
  cols_->push_back(create_column_func_(id, data_type::type_unsigned_int, index_++));
}

void typed_column_serializer::serialize(const char *id, unsigned long&)
{
  cols_->push_back(create_column_func_(id, data_type::type_unsigned_long, index_++));
}

void typed_column_serializer::serialize(const char *id, float&)
{
  cols_->push_back(create_column_func_(id, data_type::type_float, index_++));
}

void typed_column_serializer::serialize(const char *id, double&)
{
  cols_->push_back(create_column_func_(id, data_type::type_double, index_++));
}

void typed_column_serializer::serialize(const char *id, bool&)
{
  cols_->push_back(create_column_func_(id, data_type::type_bool, index_++));
}

void typed_column_serializer::serialize(const char *id, char *, size_t s)
{
  cols_->push_back(create_varchar_column_func_(id, s, data_type::type_char_pointer, index_++));
}

void typed_column_serializer::serialize(const char *id, varchar_base &x)
{
  cols_->push_back(create_varchar_column_func_(id, x.capacity(), data_type::type_varchar, index_++));
}

void typed_column_serializer::serialize(const char *id, std::string &)
{
  cols_->push_back(create_column_func_(id, data_type::type_text, index_++));
}

void typed_column_serializer::serialize(const char *id, date &)
{
  cols_->push_back(create_column_func_(id, data_type::type_date, index_++));
}

void typed_column_serializer::serialize(const char *id, time &)
{
  cols_->push_back(create_column_func_(id, data_type::type_time, index_++));
}

void typed_column_serializer::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key()->serialize(id, *this);
  } else {
    std::unique_ptr<basic_identifier> pk(x.create_identifier());
    pk->serialize(id, *this);
  }
}

void typed_column_serializer::serialize(const char *id, basic_identifier &x)
{
  if (x.as_value()) {
    x.serialize(id, *this);
  } else {
    create_column_func_ = make_column<typed_identifier_column>;
    create_varchar_column_func_ = make_varchar_column<identifier_varchar_column>;
    x.serialize(id, *this);
    create_column_func_ = make_column<typed_column>;
    create_varchar_column_func_ = make_varchar_column<typed_varchar_column>;
  }
}

}
}
