#include "matador/sql/value_extractor.hpp"

namespace matador::sql {

value_extractor::value_extractor(std::vector<utils::any_type> &values)
: values_(values)
{}

void value_extractor::on_primary_key(const char *, std::string &pk, size_t s)
{
  object::data_type_traits<std::string>::bind_value(*this, 0, pk, s);
}

void value_extractor::on_revision(const char *, unsigned long long int &rev)
{
  object::data_type_traits<unsigned long long int>::bind_value(*this, 0, rev);
}

void value_extractor::on_attribute(const char *, char *x, const utils::field_attributes &attr)
{
  object::data_type_traits<char*>::bind_value(*this, 0, x, attr.size());
}

void value_extractor::on_attribute(const char *, std::string &x, const utils::field_attributes &attr)
{
  object::data_type_traits<std::string>::bind_value(*this, 0, x, attr.size());
}

void value_extractor::bind(size_t /*pos*/, char i)
{
  values_.emplace_back(i);
}

void value_extractor::bind(size_t /*pos*/, short i)
{
  values_.emplace_back(i);
}

void value_extractor::bind(size_t /*pos*/, int i)
{
  values_.emplace_back(i);
}

void value_extractor::bind(size_t /*pos*/, long i)
{
  values_.emplace_back(i);
}

void value_extractor::bind(size_t /*pos*/, long long int i)
{
  values_.emplace_back(i);
}

void value_extractor::bind(size_t /*pos*/, unsigned char i)
{
  values_.emplace_back(i);
}

void value_extractor::bind(size_t /*pos*/, unsigned short i)
{
  values_.emplace_back(i);
}

void value_extractor::bind(size_t /*pos*/, unsigned int i)
{
  values_.emplace_back(i);
}

void value_extractor::bind(size_t /*pos*/, unsigned long i)
{
  values_.emplace_back(i);
}

void value_extractor::bind(size_t /*pos*/, unsigned long long int i)
{
  values_.emplace_back(i);
}

void value_extractor::bind(size_t /*pos*/, bool b)
{
  values_.emplace_back(b);
}

void value_extractor::bind(size_t /*pos*/, float d)
{
  values_.emplace_back(d);
}

void value_extractor::bind(size_t /*pos*/, double d)
{
  values_.emplace_back(d);
}

void value_extractor::bind(size_t /*pos*/, const time &time)
{
  values_.emplace_back(time);
}

void value_extractor::bind(size_t /*pos*/, const date &date)
{
  values_.emplace_back(date);
}

void value_extractor::bind(size_t /*pos*/, const char *string)
{
  values_.emplace_back(string);
}

void value_extractor::bind(size_t /*pos*/, const char *string, size_t /*size*/)
{
  values_.emplace_back(string);
}

void value_extractor::bind(size_t /*pos*/, const std::string &string)
{
  values_.emplace_back(string);
}

void value_extractor::bind(size_t /*pos*/, const std::string &x, size_t /*size*/)
{
  values_.emplace_back(x);
}

void value_extractor::bind(size_t /*pos*/, const utils::blob &blob)
{
  values_.emplace_back(blob);
}

}