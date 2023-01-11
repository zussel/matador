#include "matador/json/json_identifier_serializer.hpp"
#include "matador/utils/basic_identifier.hpp"

namespace matador {

namespace detail {

template < class T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
void to_json(T &val, std::string &out)
{
  out.append(std::to_string(val));
}

}

std::string json_identifier_serializer::serialize(basic_identifier &x)
{
  json_.clear();
//  x.serialize("", *this);
  return json_;
}

void json_identifier_serializer::on_attribute(const char*, char &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, short &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, int &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, long &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, long long &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, unsigned char &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, unsigned short &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, unsigned int &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, unsigned long &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, unsigned long long &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, bool &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, float &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, double &x)
{
  detail::to_json(x, json_);
}

void json_identifier_serializer::on_attribute(const char*, char *x, size_t len)
{
  json_.append(x, len);
}

void json_identifier_serializer::on_attribute(const char*, std::string &x)
{
  json_.append(x);
}

void json_identifier_serializer::on_attribute(const char*, std::string &x, size_t len)
{
  json_.append(x.c_str(), len);
}

}
