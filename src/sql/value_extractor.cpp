#include "matador/sql/value_extractor.hpp"

namespace matador::sql {

value_extractor::value_extractor(std::vector<utils::any_type> &values)
: values_(values)
{}

void value_extractor::on_primary_key(const char *, std::string &pk, size_t s)
{
  utils::data_type_traits<std::string>::bind_value(*this, 0, pk, s);
}

void value_extractor::on_revision(const char *, unsigned long long int &rev)
{
  utils::data_type_traits<unsigned long long int>::bind_value(*this, 0, rev);
}

void value_extractor::on_attribute(const char *, char *x, const utils::field_attributes &attr)
{
  utils::data_type_traits<char*>::bind_value(*this, 0, x, attr.size());
}

void value_extractor::on_attribute(const char *, std::string &x, const utils::field_attributes &attr)
{
  utils::data_type_traits<std::string>::bind_value(*this, 0, x, attr.size());
}

void value_extractor::write_value(size_t /*pos*/, const char &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const short &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const int &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const long &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const long long int &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const unsigned char &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const unsigned short &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const unsigned int &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const unsigned long &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const unsigned long long int &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const bool &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const float &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const double &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const time &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const date &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const char *x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const char *x, size_t /*size*/)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const std::string &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const std::string &x, size_t /*size*/)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const utils::blob &x)
{
  values_.emplace_back(x);
}

void value_extractor::write_value(size_t /*pos*/, const utils::value &/*x*/, size_t /*size*/)
{
//  values_.emplace_back(x);
}

}