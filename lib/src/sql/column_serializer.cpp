//
// Created by sascha on 2/25/16.
//
#include "sql/column_serializer.hpp"
#include "sql/column.hpp"

#include "tools/identifiable_holder.hpp"
#include "tools/varchar.hpp"

namespace oos {
namespace detail {

column_serializer::column_serializer(oos::sql &d)
  : sql_(d)
{}

void column_serializer::serialize(const char *id, char&)
{
  sql_.append(new typed_column(id, type_char, index_++, false));
}

void column_serializer::serialize(const char *id, short&)
{
  sql_.append(new typed_column(id, type_short, index_++, false));
}

void column_serializer::serialize(const char *id, int&)
{
  sql_.append(new typed_column(id, type_int, index_++, false));
}

void column_serializer::serialize(const char *id, long&)
{
  sql_.append(new typed_column(id, type_long, index_++, false));
}

void column_serializer::serialize(const char *id, unsigned char&)
{
  sql_.append(new typed_column(id, type_unsigned_char, index_++, false));
}

void column_serializer::serialize(const char *id, unsigned short&)
{
  sql_.append(new typed_column(id, type_unsigned_short, index_++, false));
}

void column_serializer::serialize(const char *id, unsigned int&)
{
  sql_.append(new typed_column(id, type_unsigned_int, index_++, false));
}

void column_serializer::serialize(const char *id, unsigned long&)
{
  sql_.append(new typed_column(id, type_unsigned_long, index_++, false));
}

void column_serializer::serialize(const char *id, float&)
{
  sql_.append(new typed_column(id, type_float, index_++, false));
}

void column_serializer::serialize(const char *id, double&)
{
  sql_.append(new typed_column(id, type_double, index_++, false));
}

void column_serializer::serialize(const char *id, bool&)
{
  sql_.append(new typed_column(id, type_bool, index_++, false));
}

void column_serializer::serialize(const char *id, char *, size_t s)
{
  sql_.append(new varchar_column(id, s, type_char_pointer, index_++, false));
}

void column_serializer::serialize(const char *id, varchar_base &x)
{
  sql_.append(new varchar_column(id, x.capacity(), type_varchar, index_++, false));
}

void column_serializer::serialize(const char *id, std::string &)
{
  sql_.append(new typed_column(id, type_text, index_++, false));
}

void column_serializer::serialize(const char *id, date &)
{
  sql_.append(new typed_column(id, type_date, index_++, false));
}

void column_serializer::serialize(const char *id, time &)
{
  sql_.append(new typed_column(id, type_time, index_++, false));
}

void column_serializer::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key()->serialize(id, *this);
  }
//  serialize(id, type_long);
}

void column_serializer::serialize(const char *, basic_identifier &)
{
  // Todo: determine identifier type
//  new identifier_column()
}

}
}
