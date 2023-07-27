#include "matador/sql/value_column_serializer.hpp"
#include "matador/utils/identifiable_holder.hpp"

namespace matador {
namespace detail {

value_column_serializer::value_column_serializer()
: value_column_identifier_serializer_(*this) {}

void value_column_serializer::on_attribute(const char *id, char &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, short &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, int &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, long &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, long long &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, unsigned char &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, unsigned short &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, unsigned int &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, unsigned long &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, unsigned long long &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, float &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, double &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, bool &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, char *x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, std::string &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, date &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_attribute(const char *id, time &x, long size)
{
  cols_->push_back(make_value_column(id, x, size));
}

void value_column_serializer::on_belongs_to(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key().serialize(value_column_identifier_serializer_);
  } else {
    cols_->push_back(std::make_shared<value_column>(id, new null_value));
  }
}

void value_column_serializer::on_has_one(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key().serialize(value_column_identifier_serializer_);
  } else {
    cols_->push_back(std::make_shared<value_column>(id, new null_value));
  }
}

}
}
