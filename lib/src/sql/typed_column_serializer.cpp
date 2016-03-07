//
// Created by sascha on 2/25/16.
//
#include "sql/typed_column_serializer.hpp"

#include "tools/identifiable_holder.hpp"
#include "tools/basic_identifier.hpp"
#include "tools/varchar.hpp"

namespace oos {
namespace detail {

template < class T, int type >
std::shared_ptr<column> make_column();

template < class T, int type >
std::shared_ptr<column> make_column();

typed_column_serializer::typed_column_serializer(oos::sql &d)
  : sql_(d)
{}

void typed_column_serializer::serialize(const char *id, char&)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_char, index_++, false));
}

void typed_column_serializer::serialize(const char *id, short&)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_short, index_++, false));
}

void typed_column_serializer::serialize(const char *id, int&)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_int, index_++, false));
}

void typed_column_serializer::serialize(const char *id, long&)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_long, index_++, false));
}

void typed_column_serializer::serialize(const char *id, unsigned char&)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_unsigned_char, index_++, false));
}

void typed_column_serializer::serialize(const char *id, unsigned short&)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_unsigned_short, index_++, false));
}

void typed_column_serializer::serialize(const char *id, unsigned int&)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_unsigned_int, index_++, false));
}

void typed_column_serializer::serialize(const char *id, unsigned long&)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_unsigned_long, index_++, false));
}

void typed_column_serializer::serialize(const char *id, float&)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_float, index_++, false));
}

void typed_column_serializer::serialize(const char *id, double&)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_double, index_++, false));
}

void typed_column_serializer::serialize(const char *id, bool&)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_bool, index_++, false));
}

void typed_column_serializer::serialize(const char *id, char *, size_t s)
{
  cols_->push_back(std::make_shared<varchar_column>(id, s, type_char_pointer, index_++, false));
}

void typed_column_serializer::serialize(const char *id, varchar_base &x)
{
  cols_->push_back(std::make_shared<varchar_column>(id, x.capacity(), type_varchar, index_++, false));
}

void typed_column_serializer::serialize(const char *id, std::string &)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_text, index_++, false));
}

void typed_column_serializer::serialize(const char *id, date &)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_date, index_++, false));
}

void typed_column_serializer::serialize(const char *id, time &)
{
  cols_->push_back(std::make_shared<typed_column>(id, type_time, index_++, false));
}

void typed_column_serializer::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key()->serialize(id, *this);
  }
//  serialize(id, type_long);
}

void typed_column_serializer::serialize(const char *id, basic_identifier &x)
{
  generate_pk_column_ = true;
  x.serialize(id, *this);
  generate_pk_column_ = false;
//  new identifier_column()
}

}
}
