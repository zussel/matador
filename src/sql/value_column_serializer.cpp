//
// Created by sascha on 03.04.16.
//

#include "oos/sql/value_column_serializer.hpp"
#include "oos/utils/identifiable_holder.hpp"
#include "oos/utils/basic_identifier.hpp"

namespace oos {
namespace detail {

value_column_serializer::value_column_serializer()
{ }

template < class T >
std::shared_ptr<value_column<T>> make_shared_value_column(const char *col, T &val)
{
  return std::make_shared<value_column<T>>(col, val);
}

std::shared_ptr<value_column<char*>> make_shared_value_column(const char *col, char* &val, size_t s)
{
  return std::make_shared<value_column<char*>>(col, val, s);
}

void value_column_serializer::serialize(const char *id, char &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, short &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, int &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, long &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, unsigned char &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, unsigned short &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, unsigned int &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, unsigned long &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, float &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, double &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, bool &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, char *x, size_t s)
{
  cols_->push_back(make_shared_value_column(id, x, s));
}

void value_column_serializer::serialize(const char *id, varchar_base &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, std::string &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, date &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, time &x)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key()->serialize(id, *this);
  } else {
    cols_->push_back(std::make_shared<value_column<null_value>>(id, new null_value));
  }
}

void value_column_serializer::serialize(const char *id, basic_identifier &x)
{
  x.serialize(id, *this);
}

}
}
