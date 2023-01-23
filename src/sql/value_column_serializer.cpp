#include "matador/sql/value_column_serializer.hpp"
#include "matador/utils/identifiable_holder.hpp"

namespace matador {
namespace detail {

value_column_serializer::value_column_serializer()
: value_column_identifier_serializer_(*this) {}

template < class T >
std::shared_ptr<value_column<T>> make_shared_value_column(const char *col, T &val)
{
  return std::make_shared<value_column<T>>(col, val);
}

template < class T >
std::shared_ptr<value_column<T>> make_shared_value_column(const char *col, T &val, long size)
{
  return std::make_shared<value_column<T>>(col, val, size);
}

std::shared_ptr<value_column<char*>> make_shared_value_column(const char *col, char* &val, long s)
{
  return std::make_shared<value_column<char*>>(col, val, s);
}

std::shared_ptr<value_column<char*>> make_shared_value_column(const char *col, char* &val)
{
  return std::make_shared<value_column<char*>>(col, val);
}

void value_column_serializer::on_attribute(const char *id, char &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, short &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, int &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, long &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, long long &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, unsigned char &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, unsigned short &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, unsigned int &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, unsigned long &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, unsigned long long &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, float &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, double &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, bool &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, char *x, long size)
{
  if (size < 0) {
    cols_->push_back(make_shared_value_column(id, x));
  } else {
    cols_->push_back(make_shared_value_column(id, x, size));
  }
}

void value_column_serializer::on_attribute(const char *id, std::string &x, long size)
{
  if (size < 0) {
    cols_->push_back(make_shared_value_column(id, x));
  } else {
    cols_->push_back(make_shared_value_column(id, x, size));
  }
}

void value_column_serializer::on_attribute(const char *id, date &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_attribute(const char *id, time &x, long /*size*/)
{
  cols_->push_back(make_shared_value_column(id, x));
}

void value_column_serializer::on_belongs_to(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key().serialize(value_column_identifier_serializer_);
  } else {
    cols_->push_back(std::make_shared<value_column<null_value>>(id, new null_value));
  }
}

void value_column_serializer::on_has_one(const char *id, identifiable_holder &x, cascade_type)
{
  if (x.has_primary_key()) {
    x.primary_key().serialize(value_column_identifier_serializer_);
  } else {
    cols_->push_back(std::make_shared<value_column<null_value>>(id, new null_value));
  }
}

void value_column_identifier_serializer::serialize(null_type_t &) {

}
}
}
